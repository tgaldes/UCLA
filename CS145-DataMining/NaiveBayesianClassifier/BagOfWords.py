import pyximport
pyximport.install()


class BagOfWords:

    def __init__(self):
        self.count = 0
        self.vocabulary = {}

    # Given a set of recipes,
    # builds vocabulary dictionary and updates count.
    def build_vocabulary(self, recipes):
        self.vocabulary = {}
        for recipe in recipes:
            for ingredient in recipe['ingredients']:
                # If ingredient not in vocabulary, map it to new index
                if ingredient not in self.vocabulary:
                    index = len(self.vocabulary)
                    self.vocabulary[ingredient] = index
        # Update vocabulary count
        self.count = len(self.vocabulary)

    # Given a set of recipes,
    # returns binary feature vectors
    # ex: [0, 0, 1, 0, ..., id, cuisine]
    def build_vectors(self, recipes):
        vectors = []
        for recipe in recipes:
            vector = [0] * self.count
            for ingredient in recipe['ingredients']:
                if ingredient in self.vocabulary:
                    # per ingredient, set feature to 1
                    vector[self.vocabulary[ingredient]] = 1
            # set id and cuisine
            vector.append(recipe['id'])
            if 'cuisine' in recipe:
                vector.append(recipe['cuisine'])
            else:
                vector.append('cuisine')
            vectors.append(vector)
        return vectors
