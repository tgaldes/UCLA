import pyximport
pyximport.install()


class NaiveBayesClassifier:

    def __init__(self):
        self.rare = 0.001
        self.cuisine_total = 0

        self.cuisine_count = {}
        self.cuisine_ingredient_count = {}

        self.prob_c = {}
        self.prob_i_given_c = {}

    # Given a set of feature vectors,
    # will train the Classifier by counting and generating conditional probabilities
    def train(self, vectors):
        for vector in vectors:
            cuisine = vector[-1]

            # increment cuisine counts
            self.cuisine_total += 1
            if cuisine not in self.cuisine_count:
                self.cuisine_count[cuisine] = 1
            else:
                self.cuisine_count[cuisine] += 1

            # increment ingredient counts
            if cuisine not in self.cuisine_ingredient_count:
                self.cuisine_ingredient_count[cuisine] = vector[:-2]
            else:
                self.cuisine_ingredient_count[cuisine] = \
                    [sum(i) for i in zip(self.cuisine_ingredient_count[cuisine], vector[:-2])]

        # calculate P(cuisine)
        for cuisine, counts in self.cuisine_count.items():
            self.prob_c[cuisine] = self.cuisine_count[cuisine] / float(self.cuisine_total)

        # calculate P(ingredient | cuisine)
        for cuisine, cuisine_counts in self.cuisine_ingredient_count.items():
            # initialize probabilities for ingredients
            if cuisine not in self.prob_i_given_c:
                self.prob_i_given_c[cuisine] = [0] * len(cuisine_counts)
            # calculate probabilities per ingredient
            for n in range(len(cuisine_counts)):
                self.prob_i_given_c[cuisine][n] = \
                    self.cuisine_ingredient_count[cuisine][n] / float(self.cuisine_count[cuisine])

    def get_p_cuisine(self, cuisine):
        return self.prob_c[cuisine] if cuisine in self.prob_c else self.rare

    def get_prob_i_given_c(self, cuisine, ingredient, positive):
        p = self.prob_i_given_c[cuisine][ingredient]
        p = p if positive else 1 - p
        # smoothing
        if p == 0:
            p = self.rare
        return p

    def get_prob_c_given_v(self, cuisine, vector):
        prob = self.get_p_cuisine(cuisine)
        for n in range(len(vector) - 2):
            prob *= self.get_prob_i_given_c(cuisine, n, vector[n])
        return prob

    def classify(self, vector):
        max_cuisine = ''
        max_p_cuisine = 0
        for cuisine, count in self.cuisine_count.items():
            p_cuisine = self.get_prob_c_given_v(cuisine, vector)
            if p_cuisine > max_p_cuisine:
                max_p_cuisine = p_cuisine
                max_cuisine = cuisine
        return max_cuisine
