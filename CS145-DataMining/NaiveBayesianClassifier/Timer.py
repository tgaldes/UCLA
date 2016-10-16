import time
import pyximport
pyximport.install()


class Timer:

    def __init__(self, message, indent=4):
        self.message = message
        self.indent = indent

    def __enter__(self):
        self.start = time.time()
        print((' ' * self.indent) + '{}'.format(self.message))
        return self

    def update(self, message):
        self.message = message

    def __exit__(self, *arguments):
        self.seconds = time.time() - self.start
        self.minutes = self.seconds / 60
        if self.minutes > 0.25:
            print((' ' * self.indent) + '{:.2f} (mins) {}'.format(self.minutes, self.message))
        else:
            print((' ' * self.indent) + '{:.1f} (secs) {}'.format(self.seconds, self.message))
