import json
import csv
import random
from Timer import Timer
from BagOfWords import BagOfWords
from NaiveBayesClassifier import NaiveBayesClassifier
import pyximport
pyximport.install()


class CuisineClassification:
    def __init__(self):
        self.bag = BagOfWords()
        self.nbc = NaiveBayesClassifier()

    def classify_check(self, train_json, train_ratio, learn_ratio, randomize=False):
        with Timer('Naive Bayes Classifier, Classify Check', indent=0) as total_t:
            with Timer('Loading Recipes for Training') as t:
                with open(train_json) as train_file:
                    recipes = json.load(train_file)
                    if randomize:
                        random.shuffle(recipes)
                    train_size = int(len(recipes) * train_ratio)
                    test_size = int(len(recipes) * learn_ratio)
                    train_recipes = recipes[:train_size]
                    test_recipes = recipes[train_size:(train_size + test_size)]
                    t.update('Loaded {}(training) and {}(testing) recipes'.format(
                        len(train_recipes), len(test_recipes)))

            with Timer('Building Bag of Words') as t:
                self.bag.build_vocabulary(train_recipes)
                t.update('Built bag with {} sized vocabulary'.format(self.bag.count))

            with Timer('Building Training Vectors') as t:
                train_vectors = self.bag.build_vectors(train_recipes)
                t.update('Built {} vectors'.format(len(train_vectors)))

            with Timer('Building Testing Vectors') as t:
                test_vectors = self.bag.build_vectors(test_recipes)
                t.update('Built {} vectors'.format(len(test_vectors)))

            with Timer('Training Bayes Classifier') as t:
                train_vectors = self.bag.build_vectors(train_recipes)
                self.nbc.train(train_vectors)
                t.update('Trained with {} vectors'.format(self.nbc.cuisine_total))

            with Timer('Making Predictions') as t:
                total = 0
                correct = 0
                for n in range(len(test_vectors)):
                    vector = test_vectors[n]
                    recipe = test_recipes[n]
                    prediction = self.nbc.classify(vector)
                    total += 1
                    if prediction == recipe['cuisine']:
                        correct += 1
                t.update('Finished {} predictions with accuracy of {:.1%}'.format(
                    total, correct / float(total)))
            total_t.update('Total')

    def classify_test(self, train_json, test_json, prediction_csv, scale):
        with Timer('Naive Bayes Classifier, Classify Test', indent=0) as total_t:
            with Timer('Loading Recipes for Training') as t:
                with open(train_json) as train_file:
                    train_recipes = json.load(train_file)
                    train_recipes = train_recipes[:int(len(train_recipes) * scale)]
                    t.update('Loaded {} training recipes'.format(len(train_recipes)))

            with Timer('Loading Recipes for Testing') as t:
                with open(test_json) as test_file:
                    test_recipes = json.load(test_file)
                    test_recipes = test_recipes[:int(len(test_recipes) * scale)]
                    t.update('Loaded {} testing recipes'.format(len(test_recipes)))

            with Timer('Building Bag of Words') as t:
                self.bag.build_vocabulary(train_recipes)
                t.update('Built bag with {} sized vocabulary'.format(self.bag.count))

            with Timer('Building Training Vectors') as t:
                train_vectors = self.bag.build_vectors(train_recipes)
                t.update('Built {} vectors'.format(len(train_vectors)))

            with Timer('Building Testing Vectors') as t:
                test_vectors = self.bag.build_vectors(test_recipes)
                t.update('Built {} vectors'.format(len(test_vectors)))

            with Timer('Training Bayes Classifier') as t:
                train_vectors = self.bag.build_vectors(train_recipes)
                self.nbc.train(train_vectors)
                t.update('Trained with {} vectors'.format(self.nbc.cuisine_total))

            with Timer('Writing Predictions') as t:
                predictions = 0
                with open(prediction_csv, "wt") as prediction_file:
                    writer = csv.writer(prediction_file)
                    writer.writerow(['id', 'cuisine'])

                    for n in range(len(test_vectors)):
                        predictions += 1
                        vector = test_vectors[n]
                        recipe_id = test_recipes[n]['id']
                        prediction = self.nbc.classify(vector)
                        writer.writerow([recipe_id, prediction])
                t.update('Wrote out {} predictions'.format(predictions))
            total_t.update('Total')
