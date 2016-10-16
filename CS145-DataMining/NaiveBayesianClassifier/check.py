from CuisineClassification import CuisineClassification
import pyximport
pyximport.install()


# Train with % of Train JSON Data,
# Test  with % of Train JSON Data,
# Randomized
cc = CuisineClassification()
cc.classify_check('data/train.json', 0.30, 0.05, False)

# EXAMPLE OUTPUT for (25%, 5%, False)

# python3 check.py
# Naive Bayes Classifier, Classify Check
# python3 check.py
# Naive Bayes Classifier, Classify Check
#     Loading Recipes for Training
#     0.1 (secs) Loaded 11932(training) and 1988(testing) recipes
#     Building Bag of Words
#     0.0 (secs) Built bag with 4725 sized vocabulary
#     Building Training Vectors
#     0.8 (secs) Built 11932 vectors
#     Building Testing Vectors
#     0.1 (secs) Built 1988 vectors
#     Training Bayes Classifier
#     9.7 (secs) Trained with 11932 vectors
#     Making Predictions
#     0.99 (mins) Finished 1988 predictions with accuracy of 73.4%
# 1.17 (mins) Total
