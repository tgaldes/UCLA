var TwitterStreamChannels = require('twitter-stream-channels');
var mysql = require('mysql');

// Initialize the MySQL connection
// Resource - https://github.com/felixge/node-mysql
var connection = mysql.createConnection({
	host: 'localhost',
	user: 'root',
	password: '',
	database: 'ticktalk'
});

connection.connect();

// Initialize the TwitterStreamChannels client
// Resource - https://www.npmjs.com/package/twitter-stream-channels
var client = new TwitterStreamChannels({
   consumer_key: process.env.TWITTER_CONSUMER_KEY,
   consumer_secret: process.env.TWITTER_CONSUMER_SECRET,
   access_token: process.env.TWITTER_ACCESS_TOKEN_KEY,
   access_token_secret: process.env.TWITTER_ACCESS_TOKEN_SECRET
});

// Keywords to listen for, organized into 'channels' by relevant stock.
//	Using arrays of strings here makes it easy to add new keywords for a stock
var channels = {
	"keywords_ba": 		['\$BA','Boeing'],
	"keywords_unh": 	['\$UNH', 'United Healthcare'],
	"keywords_wfc": 	['\$WFC', 'Wells Fargo'],
	"keywords_t": 		['\$T', 'AT&T'],
	"keywords_bp": 		['\$BP', 'BP'],
	"keywords_pcg": 	['\$PCG', 'Pacific Gas'],
	"keywords_ko": 		['\$KO', 'Coca', 'Coke'],
	"keywords_ibm": 	['\$IBM', 'IBM'],
	"keywords_msft": 	['\$MSFT', 'Microsoft'],
	"keywords_mar": 	['\$MAR', 'Marriott'],
	"keywords_atvi": 	['\$ATVI', 'Activision'],
	"keywords_ed": 		['\$ED', 'Con Edison', 'Consolidated Edison', 'Con Ed'],
	"keywords_fisv": 	['\$FISV', 'Fiserv'],
	"keywords_cern": 	['\$CERN', 'Cerner'],
	"keywords_mhk": 	['\$MHK', 'Mohawk Industries'],
	"keywords_msi": 	['\$MSI', 'Motorola Solutions']
};

// Initialize the stream channels
var stream = client.streamChannels({track:channels});

// Helper function that converts a string to an int
//	Returns 1 if input string is not null
//	Returns 0 if input string is null
function getBoolVal(str) {
	return str ? 1 : 0;
}

// Helper function to make strings play nicely with MySQL
// Source: http://stackoverflow.com/questions/7744912/making-a-javascript-string-sql-friendly
function mysql_real_escape_string (str) {
	return str.replace(/[\0\x08\x09\x1a\n\r"'\\\%]/g, function (char) {
		switch (char) {
			case "\0":
				return "\\0";
			case "\x08":
				return "\\b";
			case "\x09":
				return "\\t";
			case "\x1a":
				return "\\z";
			case "\n":
				return "\\n";
			case "\r":
				return "\\r";
			case "\"":
			case "'":
			case "\\":
			case "%":
				return "\\"+char;
		}
	});
}

// Function that takes two arguments (below)
//	and stores the tweet's important fields in the database.
//
//	Converts tweet JSON objects into a format that can be used in a MySQL query.
//
//	twt: tweet streamed from Twitter API as JSON
//	sym: string containining the relevant stock symbol for the tweet
function storeTweetInDB(twt, sym) {
	// We only want to store tweets that are in English
	if (twt['lang'] == 'en') {
		var pkey = twt['id_str'];
		var tweetText = mysql_real_escape_string(twt['text']);
		var user = mysql_real_escape_string(twt['user']['id_str']);
		var name = mysql_real_escape_string(twt['user']['name']);
		var username = mysql_real_escape_string(twt['user']['screen_name']);
		var loc = "";
		
		if (twt['user']['location'] != null) {
			loc = mysql_real_escape_string(twt['user']['location']);
		}

		var description = "";
		if (twt['user']['description'] != null) {
			description = mysql_real_escape_string(twt['user']['description']);
		}
		
		var protectd = getBoolVal(twt['user']['protected']);
		var verified = getBoolVal(twt['user']['verified']);
		var followers_count = parseInt(twt['user']['followers_count']);
		var friends_count = parseInt(twt['user']['friends_count']);
		var listed_count = parseInt(twt['user']['listed_count']);
		var favourites_count = parseInt(twt['user']['favourites_count']);
		var statuses_count = parseInt(twt['user']['statuses_count']);
		var possibly_sensitive = getBoolVal(twt['possibly_sensitive']);
		var tweetTime = Math.floor(parseInt(twt['timestamp_ms']) / 1000);		// convert ms to s
		
		// If the stock symbol ($SYMBOL notation) was used in the tweet,
		//	we have a specific field to note that.
		var symbol_mentioned = 0;
		if (tweetText.indexOf(sym) > -1) {
			symbol_mentioned = 1;
		}
	
		var duplicateFlag = 0;
		connection.query('INSERT INTO tweets (tweet_id, stock_symbol, tweet_text, user_id, name, user_name, location, description, protected, verified, followers_count, friends_count, listed_count, favourites_count, statuses_count, possibly_sensitive, tweet_time, symbol_mentioned) VALUES (\''+pkey+'\',\''+sym+'\',\''+tweetText+'\',\''+user+'\',\''+name+'\',\''+username+'\',\''+loc+'\',\''+description+'\','+protectd+','+verified+','+followers_count+','+friends_count+','+listed_count+','+favourites_count+','+statuses_count+','+possibly_sensitive+',FROM_UNIXTIME('+tweetTime+'),'+symbol_mentioned+')', function(err, rows, data) {
			if (err) {
				console.log(sym + ": " + err);
				duplicateFlag = 1;
			}
		});
		connection.query('UPDATE tweets SET stock_symbol=\''+sym+'\' WHERE tweet_id=\''+pkey+'\'', function(err, rows, data) {
			if (err) {
				console.log(err);
			}
		});
	}
}

// Begin streaming tweets
//	Using a different stream for each stock that we are tracking
//	allows us to easily organize tweets by their stock symbol.
stream.on('channels/keywords_ba', function(tweet) {
	storeTweetInDB(tweet, "\$BA");
});

stream.on('channels/keywords_unh', function(tweet) {
	storeTweetInDB(tweet, "\$UNH");
});

stream.on('channels/keywords_wfc', function(tweet) {
	storeTweetInDB(tweet, "\$WFC");
});

stream.on('channels/keywords_t', function(tweet) {
	storeTweetInDB(tweet, "\$T");
});

stream.on('channels/keywords_bp', function(tweet) {
	storeTweetInDB(tweet, "\$BP");
});

stream.on('channels/keywords_pcg', function(tweet) {
	storeTweetInDB(tweet, "\$PCG");
});

stream.on('channels/keywords_ko', function(tweet) {
	storeTweetInDB(tweet, "\$KO");
});

stream.on('channels/keywords_ibm', function(tweet) {
	storeTweetInDB(tweet, "\$IBM");
});

stream.on('channels/keywords_msft', function(tweet) {
	storeTweetInDB(tweet, "\$MSFT");
});

stream.on('channels/keywords_mar', function(tweet) {
	storeTweetInDB(tweet, "\$MAR");
});

stream.on('channels/keywords_atvi', function(tweet) {
	storeTweetInDB(tweet, "\$ATVI");
});

stream.on('channels/keywords_ed', function(tweet) {
	storeTweetInDB(tweet, "\$ED");
});

stream.on('channels/keywords_fisv', function(tweet) {
	storeTweetInDB(tweet, "\$FISV");
});

stream.on('channels/keywords_cern', function(tweet) {
	storeTweetInDB(tweet, "\$CERN");
});

stream.on('channels/keywords_mhk', function(tweet) {
	storeTweetInDB(tweet, "\$MHK");
});

stream.on('channels/keywords_msi', function(tweet) {
	storeTweetInDB(tweet, "\$MSI");
});
