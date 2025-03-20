import csv
import math
import random

def load_csv(filename):
    with open(filename, 'r') as file:
        lines = csv.reader(file)
        next(lines)
        dataset = []
        for row in lines:
            dataset.append([float(x) for x in row])
    return dataset


def split_dataset(dataset, split_ratio=0.7):
    random.shuffle(dataset)
    train_size = int(len(dataset) * split_ratio)
    return dataset[:train_size], dataset[train_size:]

def separate_by_class(dataset):
    separated = {}
    for i in range(len(dataset)):
        vector = dataset[i]
        class_value = vector[-1]
        if class_value not in separated:
            separated[class_value] = []
        separated[class_value].append(vector)
    return separated

def mean(numbers):
    return sum(numbers)/float(len(numbers))

def variance(numbers, mean_val):
    return sum([(x-mean_val)**2 for x in numbers]) / float(len(numbers)-1)

def summarize_dataset(dataset):
    summaries = [(mean(attribute), variance(attribute, mean(attribute))) for attribute in zip(*dataset)]
    del summaries[-1]
    return summaries

def summarize_by_class(dataset):
    separated = separate_by_class(dataset)
    summaries = {}
    for class_value, instances in separated.items():
        summaries[class_value] = summarize_dataset(instances)
    return summaries

def calculate_gaussian_probability(x, mean, var):
    if var == 0:
        return 1 if x == mean else 0
    exponent = math.exp(-((x - mean) ** 2 / (2 * var)))
    return (1 / math.sqrt(2 * math.pi * var)) * exponent

def calculate_class_probabilities(summaries, input_vector):
    probabilities = {}
    for class_value, class_summaries in summaries.items():
        probabilities[class_value] = 1
        for i in range(len(class_summaries)):
            mean_val, var = class_summaries[i]
            x = input_vector[i]
            probabilities[class_value] *= calculate_gaussian_probability(x, mean_val, var)
    return probabilities

def predict(summaries, input_vector):
    probabilities = calculate_class_probabilities(summaries, input_vector)
    best_label, best_prob = None, -1
    for class_value, probability in probabilities.items():
        if best_label is None or probability > best_prob:
            best_prob = probability
            best_label = class_value
    return best_label

def get_predictions(summaries, test_set):
    predictions = []
    for i in range(len(test_set)):
        result = predict(summaries, test_set[i])
        predictions.append(result)
    return predictions

def get_accuracy(test_set, predictions):
    correct = 0
    for i in range(len(test_set)):
        if test_set[i][-1] == predictions[i]:
            correct += 1
    return (correct / float(len(test_set))) * 100.0


filename = 'pima-indians-diabetes.csv'
dataset = load_csv(filename)
train_set, test_set = split_dataset(dataset, split_ratio=0.7)
summaries = summarize_by_class(train_set)
predictions = get_predictions(summaries, test_set)
accuracy = get_accuracy(test_set, predictions)
print(f"Accuracy on test data: {accuracy:.2f}%")
