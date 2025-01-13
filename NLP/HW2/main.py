import pandas as pd
import torch
import os
import gc
from transformers import (
    AutoTokenizer,
    AutoModelForSequenceClassification,
    AutoModelForCausalLM,
    TrainingArguments,
    Trainer,
    BitsAndBytesConfig
)
from peft import (
    LoraConfig,
    get_peft_model,
    prepare_model_for_kbit_training,
    TaskType
)
from datasets import Dataset
from sklearn.metrics import precision_recall_fscore_support, accuracy_score
from data_handler import load_and_prepare_data, prepare_datasets
from evaluation import evaluate_models
from visualization import plot_results
from huggingface_hub import login
import psutil


def get_memory_usage():
    process = psutil.Process(os.getpid())
    return process.memory_info().rss / 1024 / 1024


def clear_memory():
    gc.collect()
    if torch.cuda.is_available():
        torch.cuda.empty_cache()


class ParliamentaryClassifier:
    def __init__(self, model_name="xlm-roberta-base", num_labels=2):
        self.device = 'cuda' if torch.cuda.is_available() else 'cpu'
        self.tokenizer = AutoTokenizer.from_pretrained(model_name)

        bnb_config = BitsAndBytesConfig(
            load_in_4bit=True,
            bnb_4bit_use_double_quant=True,
            bnb_4bit_quant_type="nf4",
            bnb_4bit_compute_dtype=torch.bfloat16,
            llm_int8_skip_modules=["classifier"]
        )

        self.model = AutoModelForSequenceClassification.from_pretrained(
            model_name,
            num_labels=num_labels,
            quantization_config=bnb_config,
            device_map='auto',
            torch_dtype=torch.bfloat16
        )

        if hasattr(self.model, 'classifier'):
            self.model.classifier = self.model.classifier.to(torch.float32)

        self.model.gradient_checkpointing_enable()
        self.model = prepare_model_for_kbit_training(self.model)

        lora_config = LoraConfig(
            r=16,
            lora_alpha=32,
            target_modules=["query", "key", "value"],
            lora_dropout=0.05,
            bias="none",
            task_type=TaskType.SEQ_CLS,
            inference_mode=False,
            modules_to_save=None
        )

        self.model = get_peft_model(self.model, lora_config)

        self.model.print_trainable_parameters()

    def train(self, train_texts, train_labels, val_texts, val_labels, checkpoint_path=None):
        clear_memory()

        train_encodings = self.tokenize_data(train_texts)
        val_encodings = self.tokenize_data(val_texts)

        train_labels = torch.tensor(train_labels, dtype=torch.long)
        val_labels = torch.tensor(val_labels, dtype=torch.long)

        train_dataset = Dataset.from_dict({
            'input_ids': train_encodings['input_ids'],
            'attention_mask': train_encodings['attention_mask'],
            'labels': train_labels
        })

        val_dataset = Dataset.from_dict({
            'input_ids': val_encodings['input_ids'],
            'attention_mask': val_encodings['attention_mask'],
            'labels': val_labels
        })

        training_args = TrainingArguments(
            output_dir="./results",
            evaluation_strategy="steps",
            eval_steps=100,
            save_steps=500,
            per_device_train_batch_size=8,
            per_device_eval_batch_size=8,
            num_train_epochs=5,
            logging_dir="./logs",
            logging_steps=50,
            gradient_accumulation_steps=4,
            warmup_ratio=0.1,
            weight_decay=0.01,
            learning_rate=2e-4,
            load_best_model_at_end=True,
            metric_for_best_model='f1',
            save_total_limit=2,
            fp16=True,
            optim="paged_adamw_32bit",
            lr_scheduler_type="cosine",
            seed=42,
            report_to=[],
        )

        trainer = Trainer(
            model=self.model,
            args=training_args,
            train_dataset=train_dataset,
            eval_dataset=val_dataset,
            compute_metrics=self.compute_metrics,
        )

        trainer.train(resume_from_checkpoint=checkpoint_path)

        self.model.save_pretrained("./trained_adapter")
        return trainer

    def compute_metrics(self, pred):
        labels = pred.label_ids
        preds = pred.predictions.argmax(-1)
        precision, recall, f1, _ = precision_recall_fscore_support(
            labels, preds, average='binary'
        )
        acc = accuracy_score(labels, preds)
        return {
            'accuracy': acc,
            'f1': f1,
            'precision': precision,
            'recall': recall
        }

    def tokenize_data(self, texts, batch_size=16):
        all_encodings = {'input_ids': [], 'attention_mask': []}

        for i in range(0, len(texts), batch_size):
            batch_texts = texts[i:i + batch_size]
            encodings = self.tokenizer(
                batch_texts,
                padding=True,
                truncation=True,
                max_length=256,
                return_tensors="pt"
            )

            all_encodings['input_ids'].append(encodings['input_ids'])
            all_encodings['attention_mask'].append(encodings['attention_mask'])
            clear_memory()

        return {
            'input_ids': torch.cat(all_encodings['input_ids']),
            'attention_mask': torch.cat(all_encodings['attention_mask'])
        }


class LlamaZeroShot:
    def __init__(self, model_name="meta-llama/Llama-3.1-8B"):
        login("access_token")
        self.tokenizer = AutoTokenizer.from_pretrained(model_name)

        if self.tokenizer.pad_token is None:
            self.tokenizer.pad_token = self.tokenizer.eos_token
            self.tokenizer.pad_token_id = self.tokenizer.eos_token_id

        bnb_config = BitsAndBytesConfig(
            load_in_4bit=True,
            bnb_4bit_use_double_quant=True,
            bnb_4bit_quant_type="nf4",
            bnb_4bit_compute_dtype=torch.bfloat16
        )

        self.model = AutoModelForCausalLM.from_pretrained(
            model_name,
            quantization_config=bnb_config,
            device_map="auto",
        )

    def predict(self, texts, task='ideology', batch_size=1):
        predictions = []

        for i in range(0, len(texts), batch_size):
            batch_texts = texts[i:i + batch_size]
            batch_predictions = []

            for text in batch_texts:
                prompt = self._create_prompt(text, task)

                inputs = self.tokenizer(
                    prompt,
                    return_tensors="pt",
                    padding=True,
                    truncation=True,
                    max_length=512
                ).to(self.model.device)

                with torch.no_grad():
                    outputs = self.model.generate(
                        inputs.input_ids,
                        attention_mask=inputs.attention_mask,
                        max_new_tokens=20,
                        temperature=0.7,
                        top_p=0.9,
                        do_sample=True,
                        pad_token_id=self.tokenizer.pad_token_id,
                        num_return_sequences=1
                    )

                response = self.tokenizer.decode(outputs[0], skip_special_tokens=True)
                prediction = self._parse_response(response, task)
                batch_predictions.append(prediction)

            predictions.extend(batch_predictions)
            clear_memory()

        return predictions

    def _create_prompt(self, text, task):
        if task == 'ideology':
            return f"""Analyze this parliamentary speech and classify if the speaker's party is left-wing (0) or right-wing (1). 
            Only respond with the number 0 or 1.

            Speech: {text}

            Classification:"""
        else:
            return f"""Analyze this parliamentary speech and classify if the speaker's party is in government (0) or opposition (1).
            Only respond with the number 0 or 1.

            Speech: {text}

            Classification:"""

    def _parse_response(self, response, task):
        numbers = [int(s) for s in response.split() if s.isdigit() and int(s) in [0, 1]]
        if numbers:
            return numbers[-1]
        else:
            return 1 if task == 'ideology' else 0


def main():
    os.system("pip install -q bitsandbytes transformers peft")

    print(f"Initial memory usage: {get_memory_usage():.2f} MB")
    df = load_and_prepare_data()

    for task in ['ideology', 'power']:
        print(f"\nRunning {task} task...")
        print(f"Memory usage before task: {get_memory_usage():.2f} MB")

        X_train, X_test, y_train, y_test = prepare_datasets(
            df, task=task, use_english=(task == 'ideology')
        )

        classifier = ParliamentaryClassifier()
        trainer = classifier.train(X_train, y_train, X_test, y_test)

        clear_memory()
        print(f"Memory usage before LLaMA: {get_memory_usage():.2f} MB")

        llama = LlamaZeroShot()
        llama_preds = llama.predict(X_test, task=task)

        fine_tuned_preds = trainer.predict(trainer.eval_dataset).predictions.argmax(-1)
        metrics = evaluate_models(y_test, fine_tuned_preds, llama_preds)
        plot_results(metrics)

        del classifier, trainer, llama
        clear_memory()
        print(f"Memory usage after task: {get_memory_usage():.2f} MB")


if __name__ == "__main__":
    main()
