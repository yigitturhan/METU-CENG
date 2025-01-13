from sklearn.metrics import confusion_matrix, classification_report
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np


def evaluate_models(true_labels, preds, zero_shot_preds=None):
    import numpy as np
    from sklearn.metrics import classification_report

    true_labels = np.array(true_labels)
    preds = np.array(preds)

    metrics = {}

    if preds is not None:
        metrics['fine_tuned'] = classification_report(
            true_labels,
            preds,
            output_dict=True,
            zero_division=0
        )

    if zero_shot_preds is not None:
        zero_shot_preds = np.array(zero_shot_preds)
        metrics['zero_shot'] = classification_report(
            true_labels,
            zero_shot_preds,
            output_dict=True,
            zero_division=0
        )

    return metrics
