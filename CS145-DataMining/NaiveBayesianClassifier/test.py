from CuisineClassification import CuisineClassification
import pyximport
pyximport.install()


# Train with 100% of Train JSON and
# Test  with 100% of Test  JSON,
# write predictions to csv file
cc = CuisineClassification()
cc.classify_test('data/train.json', 'data/test.json', 'data/sub.csv', scale=1)

# EXAMPLE OUTPUT

# python3 test.py
# Naive Bayes Classifier, Classify Test
#     Loading Recipes for Training
#     0.2 (secs) Loaded 39774 training recipes
#     Loading Recipes for Testing
#     0.1 (secs) Loaded 9944 testing recipes
#     Building Bag of Words
#     0.1 (secs) Built bag with 6714 sized vocabulary
#     Building Training Vectors
#     0.54 (mins) Built 39774 vectors
#     Building Testing Vectors
#     1.57 (mins) Built 9944 vectors
#     Training Bayes Classifier
#     16.90 (mins) Trained with 39774 vectors
#     Writing Predictions
#     9.62 (mins) Wrote out 9944 predictions
# 28.63 (mins) Total
