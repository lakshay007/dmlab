import math
import re
import os
import tarfile
import urllib.request
from collections import defaultdict, Counter


def download_20newsgroups():
    url = "http://qwone.com/~jason/20Newsgroups/20news-bydate.tar.gz"
    if not os.path.exists("20news-bydate.tar.gz"):
        print("Downloading 20 Newsgroups dataset...")
        urllib.request.urlretrieve(url, "20news-bydate.tar.gz")
        print("Download complete.")
    if not os.path.exists("20news-bydate-test") or not os.path.exists("20news-bydate-train"):
        with tarfile.open("20news-bydate.tar.gz", "r:gz") as tar:
            tar.extractall()
        print("Extraction complete.")


def load_20newsgroups_data(base_dir, categories):
    data = []
    labels = []
    label_map = {cat: i for i, cat in enumerate(categories)}
    for category in categories:
        cat_path = os.path.join(base_dir, category)
        for filename in os.listdir(cat_path):
            with open(os.path.join(cat_path, filename), encoding='latin1') as f:
                text = f.read()
                data.append(text)
                labels.append(label_map[category])
    return data, labels


def preprocess(text):
    text = text.lower()
    text = re.sub(r'[^a-z\s]', '', text)
    return text.split()


def train_multinomial_nb(X_train, y_train):
    class_word_counts = defaultdict(Counter)
    class_doc_counts = Counter()
    total_docs = len(y_train)

    for text, label in zip(X_train, y_train):
        class_doc_counts[label] += 1
        words = preprocess(text)
        class_word_counts[label].update(words)

    class_priors = {cls: math.log(class_doc_counts[cls]/total_docs) for cls in class_doc_counts}
    word_probs = {}

    for cls, word_count in class_word_counts.items():
        total_words = sum(word_count.values())
        vocab_size = len(set(word for counts in class_word_counts.values() for word in counts))
        word_probs[cls] = {
            word: math.log((count + 1) / (total_words + vocab_size))
            for word, count in word_count.items()
        }
        word_probs[cls]['<UNK>'] = math.log(1 / (total_words + vocab_size))

    return class_priors, word_probs


def predict_multinomial_nb(text, class_priors, word_probs):
    words = preprocess(text)
    class_scores = {}

    for cls in class_priors:
        score = class_priors[cls]
        for word in words:
            if word in word_probs[cls]:
                score += word_probs[cls][word]
            else:
                score += word_probs[cls]['<UNK>']
        class_scores[cls] = score

    return max(class_scores, key=class_scores.get)


def confusion_matrix(y_true, y_pred, num_classes):
    cm = [[0 for _ in range(num_classes)] for _ in range(num_classes)]
    for t, p in zip(y_true, y_pred):
        cm[t][p] += 1
    return cm


def calculate_metrics(cm):
    precision = []
    recall = []
    for i in range(len(cm)):
        tp = cm[i][i]
        fp = sum(cm[r][i] for r in range(len(cm))) - tp
        fn = sum(cm[i][c] for c in range(len(cm))) - tp
        precision.append(tp / (tp + fp) if (tp + fp) > 0 else 0)
        recall.append(tp / (tp + fn) if (tp + fn) > 0 else 0)
    return precision, recall


def evaluate_model(X_test, y_test, class_priors, word_probs, num_classes):
    y_pred = []
    for text in X_test:
        pred = predict_multinomial_nb(text, class_priors, word_probs)
        y_pred.append(pred)

    cm = confusion_matrix(y_test, y_pred, num_classes)
    correct = sum(cm[i][i] for i in range(num_classes))
    total = sum(sum(row) for row in cm)
    accuracy = (correct / total) * 100
    precision, recall = calculate_metrics(cm)

    print("Confusion Matrix:")
    for row in cm:
        print(row)
    print(f"Accuracy: {accuracy:.2f}%")
    for i, (p, r) in enumerate(zip(precision, recall)):
        print(f"Class {i} - Precision: {p:.2f}, Recall: {r:.2f}")



download_20newsgroups()
categories = ['alt.atheism', 'comp.graphics', 'sci.space', 'rec.sport.hockey']
X_train, y_train = load_20newsgroups_data('20news-bydate-train', categories)
X_test, y_test = load_20newsgroups_data('20news-bydate-test', categories)


class_priors, word_probs = train_multinomial_nb(X_train, y_train)


evaluate_model(X_test, y_test, class_priors, word_probs, len(categories))