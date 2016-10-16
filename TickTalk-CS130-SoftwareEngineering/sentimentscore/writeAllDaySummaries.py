#this file is a script that calls day summary creator repeatedly
#with a list of dates to pass in
import GetDate
import DaySummaryCreator
#all dates
dates = [
'2016-05-06',
'2016-05-07',
'2016-05-08',
'2016-05-09',
'2016-05-10',
'2016-05-11',
'2016-05-12',
'2016-05-13',
'2016-05-16',
'2016-05-17',
'2016-05-18',
'2016-05-19',
'2016-05-20',
'2016-05-23',
'2016-05-24',
'2016-05-25',
'2016-05-26',
'2016-05-27',
'2016-05-31',
'2016-06-01',
'2016-06-02',
'2016-06-03']
#get current date and append it to list
dates.append(GetDate.getDate())
for i in range(len(dates)):
    DaySummaryCreator.main(dates[i])
