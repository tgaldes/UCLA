from sys import argv
import unirest
import json
import pymysql
import decimal
import GetDate
debug = False
#global list for the field types of a row in the "scoredTweets" table
scoredTweetFieldTypes = ['text', 'float','text', 'text', 'text', 'text', 'text', 'text', 'text', 'int', 'int', 'int', 'int', 'int', 'int', 'int', 'int', 'text']

#returns an sql insert statement string to insert fieldValues as types
#indicated by the corresponding fieldTyples
#returns empty string if any field types are not 'int' or 'text' or 'float'
#or if different number of field types and values
def sqlInsert(tableName, fieldTypes, fieldValues):
    if len(fieldTypes) != len(fieldValues):
        print("different number of field types and field values")
        return ""
    insertStatement = "insert into " + tableName + " VALUES ("
    for i in range(len(fieldTypes)):
        if fieldTypes[i] == 'int':
            insertStatement += str(fieldValues[i])
            insertStatement += ", "
        elif fieldTypes[i] == 'text':
            insertStatement += "\""
            insertStatement += str(fieldValues[i])
            insertStatement += "\""
            insertStatement += ", "
        elif fieldTypes[i] == 'float':
            insertStatement += "%04f" % fieldValues[i]
            insertStatement += ", "
        else:
            return ""
   #take out the last comma and space
    insertStatement = insertStatement[:-2]
    insertStatement += ")"
    return insertStatement
def testSqlInsert():
    print(sqlInsert("testTAble", ['int', 'float', 'text'], [45, 45.69, 'hooray!']))
    print(sqlInsert("testTAble", ['bad', 'float', 'text'], [45, 45.69, 'hooray!']))
    print(sqlInsert("testTAble", ['int', 'float', 'text'], [45, 45.691345981473410, 'hooray!']))
    print(sqlInsert("testTAble", [ 'float', 'text'], [45, 45.69, 'hooray!']))


#grab table of all tweets that have null sentiment scores
#SELECT * FROM TWEETS WHERE SCORE = NULL
db = pymysql.connect(autocommit=True, host="localhost", user="root", passwd="", db="ticktalk")
cur = db.cursor()
#we don't need this line now that we've created the table
#createTable = "CREATE TABLE unscoredtweets (id INTEGER NOT NULL, text varchar(140), score FLOAT, PRIMARY KEY (id))"
#cur.execute(createTable)
createTable = "CREATE TABLE scoredtweets (tweet_id varchar(45) NOT NULL, stock_symbol varchar(45), tweet_text tinytext, user_id varchar(45), name tinytext, user_name tinytext, location tinytext, description tinytext, protected tinyint(11), verified tinyint(11), followers_count int(11), friends_count int(11), listed_count int(11), favourites_count int(11), statuses_count int(11), possibly_sensitive tinyint(1), tweet_time timestamp, score float, PRIMARY KEY (tweet_id))"
#cur.execute(createTable)
#add 10 fake tweets to the database table that 			
def addFakeTweets(cursor, tableName):
    cursor.execute("insert into " + tableName + " VALUES (1, \"this is the second tweet we are inserting\", 0)")
    cursor.execute("insert into " + tableName + " VALUES (2, \"here is another good tweet. I'm very glad this works\", 0)")
    cursor.execute("insert into " + tableName + " VALUES (3, \"this one is much worse than the sencond. much much worse\", 0)")
    cursor.execute("insert into " + tableName + " VALUES (4, \"tesla announced that two senior executives are departing\", 0)")
    cursor.execute("insert into " + tableName + " VALUES (5, \"im excited for the new product launch of the microphone 12\", 0)")
    cursor.execute("insert into " + tableName + " VALUES (6, \"aracters and a new line\", 0)")
    cursor.execute("insert into " + tableName + " VALUES (7, \"almost done. hooray!\", 0)")
    cursor.execute("insert into " + tableName + " VALUES (8, \"Now we are finished with developing the greatest dildo the world has ever seen.\", 0)")

#this prints that table named tableName in the database cursor points to
def printTable(cursor, tableName):
   cursor.execute("SELECT * FROM " + tableName)
   print(cursor.fetchall())

#return true if character is one of . , ' " : ; ! false otherwise
def isPunctuation(character):
    punctuation = [',', ',', '\'', '\"', ':', ';', '!']
    for i in range(len(punctuation)):
        if character == punctuation[i]:
            return True
    return False

#clean out the special characters and new lines, replacing them all with a space
def cleanText(text):
    for i in range(len(text)):
        if text[i].isalpha() or text[i].isdigit() or isPunctuation(text[i]):
            continue
        else:
            text = text[:i] + " " + text[i+1:]
    return text
def testCleanText():
    testStrings = ["first string", "\nsecond string", "T$hird string", "fourth string &*(a", "fifth string*"]
    for i in range(len(testStrings)):
        print("Start strings: " + testStrings[i])
        print("End string   : " + cleanText(testStrings[i]))

#this assigns a sentiment score to each tweet
#takes the dictionary, tweet, and array of words for the tweet as input
#sentiment score of the tweet
def assignSentimentScore(cleanedText):
    paramString = "{\"text\": \"" + cleanedText + "\", \"max_size\":10}"
    response = unirest.post("https://smartcontent.dataninja.net/smartcontent/tag", headers = {"X-Mashape-Key":"o3xe9ExRlumshltWECAMuuM981vFp1tX6VDjsn4DPf6hvUuKia","Accept":"application/json","Content-Type":"application/json"},params=(paramString))
    moduleDebug = 0
    if moduleDebug:
        print(response.body)
    dictionary = response.body.get('entity_list', 0)
    if dictionary != 0:
        return dictionary.get('sentiment_score', 0)

    return 0


#addFakeTweets(cur, "unscoredtweets")
#grab all tweets and assign a sentiment score to them
#tweets come from inputTableName, and go to outputTableName
def moveNewTweetsToNewDatabase(inputTableName,cursor, outputTableName, numTweets):
    queryStatement = "select tweet_id, tweet_text from " + inputTableName + " where score IS NULL limit " + str(numTweets) + ";"
    print queryStatement
    cursor.execute(queryStatement)
    global scoredTweetFieldTypes
    unscoredtweets = cur.fetchall()
    for row in unscoredtweets:
        print row
        score = assignSentimentScore(cleanText(row[1]))
#udpate score in the table based on the tweet 
        cursor.execute("update " + inputTableName + " set score=" + str(score)+ " where tweet_id = " + row[0])


#addFakeTweets(cur, "unscoredtweets")
#grab all tweets and assign a sentiment score to them
#tweets come from inputTableName, and go to outputTableName
def moveOldTweetsToNewDatabase(inputTableName,cursor, outputTableName, numTweets):
    date = GetDate.getDate()
    print date
    queryStatement = "select tweet_id, tweet_text from " + inputTableName + " where symbol_mentioned = 1 and score IS NULL and (hour(tweet_time) >= 20 or hour(tweet_time) < 14) limit " + str(numTweets) + ";"
    cursor.execute(queryStatement)
    global scoredTweetFieldTypes
    unscoredtweets = cur.fetchall()
    for row in unscoredtweets:
        print row
        score = assignSentimentScore(cleanText(row[1]))
#udpate score in the table based on the tweet 
        cursor.execute("update " + inputTableName + " set score=" + str(score)+ " where tweet_id = " + row[0])
    return len(unscoredtweets)
        

a = moveOldTweetsToNewDatabase("tweets",cur, "", 2000/31)
#moveNewTweetsToNewDatabase("tweets", cur, "", 2000 - a) 
