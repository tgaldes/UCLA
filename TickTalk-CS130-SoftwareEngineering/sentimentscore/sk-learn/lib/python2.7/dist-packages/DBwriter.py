import Predictor
import pymysql
import constants as c
import sys
sys.path.insert(0, '/~/sentimentscore/')
import stocks as s
#array that contains names of all columns that represent twitter data aggregations
updateColNames = [
'influential_tweet_id',
'issuer_name',
'symbol',
'end_price',
'time_alt',
'per_avg_daily_mentions',
'per_above_avg',
'per_below_avg',
'totalMentions',
'recommendation',
'first_hour_recommendation',
'day_recommendation']
class DBwriter:
    def __init__(self, date, writeHour):
        self.date = date
        self.writeHour = writeHour
        self.currentTableName = "recommendations"
        self.historicalTableName = "historical_performance"
#use the get date class TODO
#TODO import stocks from the sentimentscore folder
        self.a = Predictor.Predictor(self.date, s.stocks, "ticktalk", 
        "daySummaries", 
        "root", "", "localhost")
        host = 'localhost'
        password = ''
        user = 'root'
        databaseName = 'ticktalk'
        connectString = "host=\"" + host + "\", user=\"" + user + "\", passwd=\"" + password + "\", db=\"" + databaseName + "\""
        self.db = pymysql.connect(autocommit=True,host=host, user=user, passwd=password, db=databaseName)
#set up cursor
        self.cur = self.db.cursor()

#FUNCS THAT GET CALLED EXTERNALLY
#update all values of reccomendations to reflect tweet data of the most recent tweets
#adds the row if none exists for the stock
    def updateCurrentRows(self):
        for i in s.stocks:
        #get the row we'll update
            newRow = self.a.getCurrentPredictionRow(i)
            sqlStatement = ""
        #check if symbol already exists in the table
            if self.uniqueKeyCurrent(self.currentTableName, i):
                #add new row
                sqlStatement = self.sqlInsertCurrent(self.currentTableName, newRow)
            else:
                sqlStatement = self.sqlUpdateCurrent(self.currentTableName, newRow)
            self.cur.execute(sqlStatement)
                #for sql update statement
        #execute the statement
#add new row to the database of historical_recs
    def updateHistoricalPredictions(self):
        for i in s.stocks:
            newRow = self.a.getHistoricalPredictionRow(i)
            if self.uniqueKeyHistorical(self.historicalTableName, newRow[1], newRow[4]):
                sqlStatement = self.sqlInsertHistorical(self.historicalTableName, newRow)
            else:
                sqlStatement = self.sqlUpdateHistorical(self.historicalTableName, newRow)
            self.cur.execute(sqlStatement)
        
#update row of historical_recs with stocks performance


#creates insert staement for the current table
    def sqlInsertCurrent(self, tableName, fieldValues):
        insertStatement = "insert into " + tableName + " VALUES ("
        for i in range(0, 3):
            insertStatement += "\"" + fieldValues[i] + "\", "
        insertStatement += str(fieldValues[3]) + ","
        insertStatement += '\"' + fieldValues[4] + '\", '
        for i in range(5, len(fieldValues)- 2):
            insertStatement += str(fieldValues[i])
            insertStatement += ", "
        for i in range(len(fieldValues) - 2, len(fieldValues)):
            insertStatement += '\"' + fieldValues[i] + '\", '
        insertStatement += '\"' + fieldValues[len(fieldValues)-1] + '\"'
        insertStatement += ")"
        return insertStatement
#returns an insert statement for a historical prediction
    def sqlInsertHistorical(self, tableName, row):
        statement = "insert into " + tableName + " VALUES ("
        for i in range(len(row)):
            if i == 3:
                statement += str(row[i]) + ", "
            else:
                statement += "\"" + row[i] + "\", "
        statement = statement[:-2]
        statement += ")"
        return statement
#returns an update statement for a row in recommendations
    def sqlUpdateCurrent(self, tableName, row):
        global updateColNames
        updateStatement = "update " + tableName + " set "
        updateStatement += updateColNames[0] + "=\"" + row[0] + "\", "
        startIndex = 3
        for i in range(startIndex, len(row)- 2):
            if i == 4:
                updateStatement += updateColNames[4] + "=\"" + row[4] + "\", "
            else:
                updateStatement += updateColNames[i] + '=' + str(row[i])
                updateStatement += ", "
        if self.writeHour > 0:
            updateStatement += updateColNames[9] + "=\"" + row[9] + "\", "
        else:
            updateStatement += updateColNames[9] + "=\"" + row[10] + "\", "
        for i in range(len(row) - 2, len(row)):
            updateStatement += updateColNames[i+1] + "=\"" + row[i] + "\", "
       #take out the last comma and space
        updateStatement = updateStatement[:-2]

        #add the where cluase that defines our pkey
        updateStatement += " where " + updateColNames[2] + "=\"" + row[2] + "\""
        return updateStatement
#returns update statement that only updates the prediction of historical row
    def sqlUpdateHistorical(self, tableName, row):
        statement = "update " + tableName + " set "
        statement += "recommendation=\"" + row[2] + "\", "
        statement += "performance=" + str(row[3]) + " "
        statement += " where symbol=\"" + row[1] + "\" and "
        statement += "date=\"" + row[4] + "\""
        return statement

#this returns true if the fieldValue have a key that is not alreay
#in tableName, false otherwise
    def uniqueKeyHistorical(self, tableName, symbol, date):
        select = "select * from " + tableName + " where "
        select += 'symbol=\"' + symbol + '\" and '
        select += 'date=\"' + date  + '\" '
        self.cur.execute(select)
        for i in self.cur.fetchall():
            return False
        return True

#this returns true if the fieldValue have a key that is not alreay
#in tableName, false otherwise
    def uniqueKeyCurrent(self, tableName, symbol):
        select = "select * from " + tableName + " where "
        select += updateColNames[2] + '=\"' + symbol + '\" '
        self.cur.execute(select)
        for i in self.cur.fetchall():
            return False
        return True










