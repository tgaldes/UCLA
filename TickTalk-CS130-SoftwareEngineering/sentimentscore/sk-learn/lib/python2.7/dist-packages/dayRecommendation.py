import DBwriter
import GetDate
#this script creates a DBwriter object and calls updateCurrentRows once
#run before the day starts and up until the market has been open for an hour
a = DBwriter.DBwriter(GetDate.getDate(), -1)
a.updateCurrentRows()


