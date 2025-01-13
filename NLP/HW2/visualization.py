import matplotlib.pyplot as plt
import numpy as np


def plot_results(metrics):
    fig = plt.figure(figsize=(10, 6))

    metrics_to_plot = ['eval_accuracy', 'eval_precision', 'eval_recall', 'eval_f1']
    fine_tuned_scores = [metrics['fine_tuned'][metric] for metric in metrics_to_plot]

    x = np.arange(len(metrics_to_plot))
    plt.bar(x, fine_tuned_scores, width=0.35, label='Fine-tuned Model')

    plt.xlabel('Metrics')
    plt.ylabel('Score')
    plt.title('Model Performance Metrics')
    plt.xticks(x, metrics_to_plot)
    plt.legend()

    plt.savefig('model_performance.png')
    plt.close()
