import pymysql
import sys
from sys import argv
from TweetDaySummary import TweetDaySummary
from StockDaySummary import StockDaySummary
import stocks as s
#this file uses tweet and stock day summary objects to create and add rows
#to the day summary table
#New/current days are added with all stock price information as NULL
#date we are creating rows for is specified by the command line

updateColNames = [
'date',
'symbol',
'stockName',
'followers_count',
'positive_followers_count',
'negative_followers_count',
'listed_count',
'positive_listed_count',
'negative_listed_count',
'statuses_count',
'positive_statuses_count'
,'negative_statuses_count',
'total_sentiment',
'positive_sentiment',
'negative_sentiment',
'average_time',
'positive_average_time',
'negative_average_time',
'average_sentiment',
'weighted_average_sentiment_by_followers',
'wa_sentiment_by_lists',
'totalMentions',
'start_price',
'hour_change',
'percent_hour_change',
'end_price',
'day_change',
'percent_day_change']
#returns true if date is in 'YYYY-MM-DD' format, false otherwise
def checkDate(date):
    if len(date) != 10:
        return False
    if date[:4].isdigit() != True:
        return False
    if date[5:7].isdigit() != True:
        return False
    if date[8:].isdigit() != True:
        return False
    if date[4] != '-' or date[7] != '-':
        return False
    return True


#this function does and sql update by using the same field values
#as sql insert and updating where we have the same symbol and day
def sqlUpdate(tableName, fieldValues):
    updateStatement = "update " + tableName + " set "
    startIndex = 3
    #set lines
    for i in range(startIndex, len(fieldValues)):
        updateStatement += updateColNames[i] + '=' + str(fieldValues[i])
        updateStatement += ", "
   #take out the last comma and space
    updateStatement = updateStatement[:-2]

    #add the where cluase that defines our pkey
    updateStatement += " where " + updateColNames[0] + "=\"" + fieldValues[0]
    updateStatement += "\" and " + updateColNames[1] + "=\"" + fieldValues[1] + "\""
    return updateStatement

#this returns true if the fieldValue have a key that is not alreay
#in tableName, false otherwise
def uniqueKey(cur, tableName, fieldValues):
    select = "select * from " + tableName + " where "
    select += updateColNames[0] + '=\"' + fieldValues[0] + '\" and '
    select += updateColNames[1] + '=\"' + fieldValues[1] + '\" '
    print select
    cur.execute(select)
    for i in cur.fetchall():
        return False
    return True

#returns a sql insert action string for daySummaries
def sqlInsert(tableName, fieldValues):
    insertStatement = "insert into " + tableName + " VALUES (0, "
    for i in range(0, 3):
        insertStatement += "\"" + fieldValues[i] + "\","
    for i in range(3, len(fieldValues)):
        insertStatement += str(fieldValues[i])
        insertStatement += ", "
   #take out the last comma and space
    insertStatement = insertStatement[:-2]
    insertStatement += ")"
    return insertStatement

def main(arg1):
#connect to database
    db = pymysql.connect(autocommit=True, host="localhost", user="root", passwd="", db="ticktalk")
    cur = db.cursor()
#grab date
    date = arg1
#if we don't have a legit date, exit
    if not checkDate(date):
        sys.exit("Incorrectly formatted date!")
#for every stock
    for i in range(len(s.stocks)):
#create stock and tweet day summary objects
        tweetSum = TweetDaySummary(date, s.stocks[i], "ticktalk", "tweets", "root", "", "localhost")
        stockSum = StockDaySummary(date, s.stocks[i], "ticktalk", "stocks", "root", "", "localhost")
        daySummary = tweetSum.returnTweetDaySummary()
        daySummary += stockSum.returnStockDaySummary()
        print daySummary
        #change around the position of stockName
        daySummary.insert(2, daySummary[-7])
        del daySummary[-7]

        #delete/update database
        if uniqueKey(cur, "daySummaries", daySummary):
            cur.execute(sqlInsert("daySummaries", daySummary))
        else:
            cur.execute(sqlUpdate("daySummaries", daySummary))



