import Predictor
import stocks as s
a = Predictor.Predictor('2016-06-03', s.stocks, "ticktalk", 
"daySummaries", 
"root", "", "localhost")
print a.getSignedDailyMentions('MSFT', True)
print a.getSignedDailyMentions('MSFT', False)
