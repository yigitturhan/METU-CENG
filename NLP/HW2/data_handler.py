import requests
import pandas as pd
from sklearn.model_selection import train_test_split

def load_and_prepare_data(task='orientation', country_code='at'):
    file_path = f"data/train/{task}/orientation-{country_code}-train.tsv"

    df = pd.read_csv(file_path, sep='\t')

    df = df[df['id'].str.startswith(country_code)]
    return df


def prepare_datasets(df, task='ideology', use_english=True):
    text_col = 'text_en' if use_english else 'text'
    X = df[text_col].tolist()
    y = df['label'].tolist()

    X_train, X_test, y_train, y_test = train_test_split(
        X, y, test_size=0.1, stratify=y, random_state=42
    )

    return X_train, X_test, y_train, y_test


def analyze_data_distribution(df, country_code='at'):
    country_data = df[df['id'].str.startswith(country_code)]

    ideology_dist = country_data['label'].value_counts(normalize=True)

    gender_dist = country_data['sex'].value_counts(normalize=True)

    return {
        'total_samples': len(country_data),
        'ideology_distribution': ideology_dist.to_dict(),
        'gender_distribution': gender_dist.to_dict()
    }
