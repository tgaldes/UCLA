import pymysql
import datetime
class StockDaySummary:
#this is a class that generates day summary information for stocks
#day summaray info includes the first hour delta, first hour % delta
#day delta, day % delta, date in question, stock name, start price, and end price
#date is in form 'YYYY-MM-DD'
    
    errorList = [-10,-10,-10,-10,-10,-10,-10]

    def __init__(self, day, stockSymbol, databaseName, tableName, user, password, host):
        self.date = day
        self.stockSymbol = stockSymbol
        self.database = databaseName
        self.__tableName = tableName
        self.firstHourD = 0
        self.firstHourPD = 0
        self.dayD = 0
        self.dayPD = 0
        self.startPrice = 0
        self.endPrice = 0

        #connect to database
        connectString = "host=\"" + host + "\", user=\"" + user + "\", passwd=\"" + password + "\", db=\"" + databaseName + "\""
        self.db = pymysql.connect(host=host, user=user, passwd=password, db=databaseName)
        #self.db = pymysql.connect(connectString)
#set up cursor
        self.cur = self.db.cursor()

    #this is for when we don't have the first hours data yet
    def errorReturnList(self):
        self.errorList[0] = self.getStockName()
        return self.errorList

#this adds n nulls to the list that is passed in
    def addNNullsToList(self, n, addNullTo):
        for i in range(n):
            addNullTo.append(-10)
        return addNullTo
#this makes call to other modules to get the stock data and then does computations, and returns a list of form [startPrice, endPrice, dayD, dayPD, firstHourD, firstHourPD]
#return list with every category null on error
    def returnStockDaySummary(self):
        stockDaySummary = []        
        stockPrices = self.getStockInfo()
#TODO: return first hour data and current price as end of day data if market is currently open
        if len(stockPrices) == 3:
            stockDaySummary.append(str(self.getStockName()))
#add start price
            stockDaySummary.append(stockPrices[0])
#change in price over the first hour
            stockDaySummary.append(stockPrices[1] - stockPrices[0])
#% change in price over the first hour
            stockDaySummary.append((stockPrices[1] - stockPrices[0])/stockPrices[1])
            #end price
            stockDaySummary.append(stockPrices[2])
#change in price over the whole day
            stockDaySummary.append(stockPrices[2] - stockPrices[0])
#% change in price over the whole day
            stockDaySummary.append((stockPrices[2] - stockPrices[0])/stockPrices[2])
            #below line was used for testing purposes
            #stockDaySummary = self.addNNullsToList(3, stockDaySummary[:4])
            return stockDaySummary
        #only first hour of data is available
        elif len(stockPrices) == 2:
            stockDaySummary.append(str(self.getStockName()))
            #add start price
            stockDaySummary.append(stockPrices[0])
#change in price over the first hour
            stockDaySummary.append(stockPrices[1] - stockPrices[0])
#% change in price over the first hour
            stockDaySummary.append((stockPrices[1] - stockPrices[0])/stockPrices[1])
            stockDaySummary = self.addNNullsToList(3, stockDaySummary)
            return stockDaySummary
        print("first hour of today's stock prices not available yet")
        return self.errorReturnList()
        
#this function returns the stock name using the member variable symbol
    def getStockName(self):
            queryString = "select issuer_name from " + self.__tableName + " where symbol = \"" + self.stockSymbol + "\" group by issuer_name"
            self.cur.execute(queryString)
            for row in self.cur.fetchall():
                return row[0]

#this function is responsible for making queries and returning a list of
#form [startPrice, endPrice, firstHourPrice]
    def getStockInfo(self):
#if is for when we didn't grab price until the first hour
        if self.date < '2016-05-14':
#until we start grabbing the price at 13:30 we'll treat hour 14 as the open
#constants that define which hours are open, firstHour, and close
            openHour = 14
            firstHour = 15
            closeHour = 20
            stockPrices = [] 
#compute the timestamp of the next day
            nextDay = self.date[:-2] + "%02d" % (int(self.date[-2:]) + 1)
            queryString = "select price, time_alt from " + self.__tableName + " where symbol = \"" + self.stockSymbol + "\" and time_alt >= \"" + self.date + "\" and time_alt < \"" + nextDay + "\""
            self.cur.execute(queryString)
            for row in self.cur.fetchall():
                if row[1].hour == openHour:
                    stockPrices.append(row[0])
                elif row[1].hour == firstHour:
                    stockPrices.append(row[0])
                elif row[1].hour == closeHour:
                    stockPrices.append(row[0])
                    break
            return stockPrices
        else:
            openHour = 13
            firstHour = 14
            closeHour = 20
            halfHour = 30
            stockPrices = [] 
#compute the timestamp of the next day
            nextDay = self.getNextDay()
            queryString = "select price, time_alt from " + self.__tableName + " where symbol = \"" + self.stockSymbol + "\" and time_alt >= \"" + self.date + "\" and time_alt < \"" + nextDay + "\""
            self.cur.execute(queryString)
            for row in self.cur.fetchall():
                if row[1].hour == openHour and row[1].minute > halfHour:
                    stockPrices.append(row[0])
                elif row[1].hour == firstHour and row[1].minute > halfHour:
                    stockPrices.append(row[0])
                elif row[1].hour == closeHour:
                    stockPrices.append(row[0])
                    break
            return stockPrices

    def getNextDay(self):
        year = int(self.date[:4])
        month = int(self.date[5:7])
        day = int(self.date[8:])
        date = [year, month, day]
        daysInEachMonth = [31,28,31,30,31,30,31,31,30,31,30,31]
#check if last day of month and year
        if date[2] == 31 and date[1] == 12:
            date[0] += 1
            date[2] = 1
            date[1] = 1
#check if last day of month
        elif date[2] == daysInEachMonth[date[1] - 1]:
            date[1] += 1
            date[2] = 1
        else:
            date[2] += 1
            toReturn = str(date[0]) + '-' + str(date[1]) + '-' + str(date[2])
        return toReturn
    def test(self):
        print self.returnStockDaySummary()

#a = StockDaySummary('2016-06-01','ATVI','ticktalk','stocks','root','','localhost')
#a.test()
