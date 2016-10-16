#returns the date of the time it was called in the form 'YYYY-MM-DD'
import datetime
def getDate():
    currentDate = datetime.date
    dateString = datetime.date.today().strftime("%Y")
    dateString += "-" + datetime.date.today().strftime("%m")
    dateString += "-" + datetime.date.today().strftime("%d")
    return dateString
