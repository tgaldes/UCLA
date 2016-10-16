#this file is a script that calls day summary creator repeatedly
#with a list of dates to pass in
import DaySummaryCreator
import GetDate
#list of stocks that we will query from our database
DaySummaryCreator.main(GetDate.getDate())
#for i in range(len(dates)):
 #   DaySummaryCreator.main(dates[i])
