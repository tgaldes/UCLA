# TickTalk - Stock Analysis Using Twitter
## Project for CS 130 - *UCLA Spring 2016*

### Using TickTalk

TickTalk is a Web application.  To use it, simply visit [the website](http://52.27.111.35/).

### Running the project

Running the backend of TickTalk must be done on our server because much of the code depends on having access to our database.
Additionally, our Twitter API keys are stored on the server and are not in this repository, for security reasons.

The software that our system runs includes:
* Linux (operating system)
* Apache (serving the front-end website)
* MySQL (databases)
* PHP (create web elements using data from the databases)
* [Twitter Bootstrap](http://getbootstrap.com/2.3.2/) (used to simplify website styling and responsiveness)
* [Node.js](https://nodejs.org/en/) (running data-collecting scripts)
* Python (generating sentiment scores)

We use multiple Node.js modules:
* [request](https://github.com/request/request)
* [mysql](https://github.com/felixge/node-mysql)
* [twitter-stream-channels](https://www.npmjs.com/package/twitter-stream-channels)

To install these modules (after installing Node.js):
* `$ npm install request`
* `$ npm install mysql`
* `$ npm install twitter-stream-channels`

We use `$ crontab -e` to set intervals at which to run the scripts that determine sentiment scores and collect stock quotes from Yahoo! Finanace.

### Resources:

Node Twitter streaming APIs:
* https://www.npmjs.com/package/twitter (used in older versions of the project)
* https://www.npmjs.com/package/twitter-stream-channels (used in latest version of the project)
* https://github.com/topheman/twitter-stream-channels

Amazon Web Services:
* http://docs.aws.amazon.com/AWSEC2/latest/UserGuide/AccessingInstances.html
* http://docs.aws.amazon.com/AWSEC2/latest/UserGuide/AccessingInstancesLinux.html
* http://docs.aws.amazon.com/AWSEC2/latest/UserGuide/install-LAMP.html
* http://stackoverflow.com/questions/26245942/how-do-i-leave-node-js-server-on-ec2-running-forever

Linux (`screen`, `crontab`, etc.):
* http://unix.stackexchange.com/questions/74695/where-do-i-set-followsymlinks
* http://stackoverflow.com/questions/210120/remove-a-symlink-to-a-directory
* http://www.thegeekstuff.com/2010/10/linux-ln-command-examples/
* https://www.linux.com/learn/how-move-files-using-linux-commands-or-file-managers
* http://superuser.com/questions/295591/what-is-the-meaning-of-chmod-666
* http://stackoverflow.com/questions/19904992/mysqldump-without-password-in-crontab
* http://askubuntu.com/questions/85558/verify-if-crontab-works
* http://superuser.com/questions/81262/how-to-execute-shell-script-via-crontab
* http://stackoverflow.com/questions/7194102/node-js-script-not-executing-from-crontab
* http://askubuntu.com/questions/54364/how-do-you-set-the-timezone-for-crontab
* http://www.thegeekstuff.com/2009/06/15-practical-crontab-examples/
* http://crontab.guru/#05_*_*_*_1-5
* http://askubuntu.com/questions/8653/how-to-keep-processes-running-after-ending-ssh-session
* http://stackoverflow.com/questions/537942/how-to-list-running-screen-sessions
* https://www.rackaid.com/blog/linux-screen-tutorial-and-how-to/

JavaScript:
* https://developer.mozilla.org/en-US/docs/Web/JavaScript/Guide/Regular_Expressions
* http://www.w3schools.com/js/js_strings.asp
* https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Statements/const
* http://stackoverflow.com/questions/29763557/syntaxerror-unexpected-token-o-at-object-parse-native-angularjs
* http://stackoverflow.com/questions/18130081/what-is-causing-uncaught-syntaxerror-unexpected-token-o-with-parsejson-an
* http://stackoverflow.com/questions/29570903/tweets-stream-error-syntaxerror-unexpected-token-e-nodejs
* http://stackoverflow.com/questions/12460378/how-to-get-json-from-url-in-javascript
* http://www.w3schools.com/jsref/jsref_parseint.asp
* http://stackoverflow.com/questions/21278234/does-parsedouble-exist-in-javascript
* https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/parseFloat
* http://stackoverflow.com/questions/13266013/when-to-close-mysql-connection-using-node-mysql
* http://stackoverflow.com/questions/11104028/process-env-node-env-is-undefined
* http://stackoverflow.com/questions/13437066/need-assistance-with-accessing-mysql-database-using-node-js
* http://stackoverflow.com/questions/874709/converting-user-input-string-to-regular-expression
* http://stackoverflow.com/questions/23432602/access-denied-while-connecting-to-mysql-through-node-js
* https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/String/match
* http://makandracards.com/makandra/15879-javascript-how-to-generate-a-regular-expression-from-a-string
* http://www.w3schools.com/js/js_functions.asp
* http://stackoverflow.com/questions/10752055/cross-origin-requests-are-only-supported-for-http-error-when-loading-a-local
* http://stackoverflow.com/questions/263965/how-can-i-convert-a-string-to-boolean-in-javascript

MySQL:
* https://dev.mysql.com/doc/refman/5.7/en/charset-syntax.html
* http://stackoverflow.com/questions/7744912/making-a-javascript-string-sql-friendly
* https://mediatemple.net/community/products/dv/204403914/default-mysql-character-set-and-collation
* http://stackoverflow.com/questions/367711/what-is-the-best-collation-to-use-for-mysql-with-php
* http://stackoverflow.com/questions/4125947/what-is-the-data-type-for-unix-timestamp-mysql
* http://dev.mysql.com/doc/refman/5.5/en/datetime.html
* http://stackoverflow.com/questions/13266013/when-to-close-mysql-connection-using-node-mysql
* https://github.com/felixge/node-mysql/issues/1333
* https://help.ubuntu.com/community/MysqlPasswordReset
* http://stackoverflow.com/questions/6081339/access-denied-for-user-rootlocalhost-using-password-yes-mysqlerror
* https://github.com/felixge/node-mysql/issues/250
* http://stackoverflow.com/questions/20353402/access-denied-for-user-testlocalhost-using-password-yes-except-root-user
* https://www.digitalocean.com/community/tutorials/a-basic-mysql-tutorial
* http://dev.mysql.com/doc/refman/5.7/en/mysql-commands.html
* http://dev.mysql.com/doc/refman/5.7/en/mysql.html
* https://dev.mysql.com/doc/refman/5.5/en/getting-information.html
* http://www.liquidweb.com/kb/select-a-mysql-database-on-linux-via-command-line/
* http://stackoverflow.com/questions/12118627/error-1049-42000-unknown-database
* http://stackoverflow.com/questions/19312907/er-bad-db-error-on-connecting-to-database-using-sequelize
* http://stackoverflow.com/questions/14333272/node-mysql-cannot-enqueue-a-query-after-calling-quit
* http://stackoverflow.com/questions/25808017/cannot-connect-nodejs-to-mysql
* https://github.com/felixge/node-mysql/issues/1211
* http://stackoverflow.com/questions/14212182/converting-unix-timestamp-to-mysql-datetime
* http://stackoverflow.com/questions/2706498/mysql-error-1064
* http://stackoverflow.com/questions/12426320/how-do-i-set-the-default-schema-for-a-user-in-mysql
* http://www.w3schools.com/sql/sql_update.asp
* http://stackoverflow.com/questions/11448068/mysql-error-code-1175-during-update-in-mysql-workbench
* http://stackoverflow.com/questions/11912221/mysql-select-by-newest-timestamp
* http://stackoverflow.com/questions/8923114/how-to-reset-auto-increment-in-mysql
* http://stackoverflow.com/questions/8537531/access-denied-for-user-rootlocalhost-using-password-no
* http://dev.mysql.com/doc/refman/5.7/en/date-and-time-functions.html
* https://dev.mysql.com/doc/refman/5.5/en/keywords.html
* http://stackoverflow.com/questions/11558418/mysql-query-insert-data-unix-timestamp-now-issue
* http://stackoverflow.com/questions/7884522/mysql-error-an-error-in-your-sql-syntax-check-the-manual-that-corresponds-to
* http://stackoverflow.com/questions/21509026/error-in-your-sql-syntax-check-the-manual-that-corresponds-to-your-mysql-server
* http://superuser.com/questions/603026/mysql-how-to-fix-access-denied-for-user-rootlocalhost
* http://stackoverflow.com/questions/2995054/access-denied-for-user-rootlocalhost-using-passwordno
* https://phpro.orgexamples/Convert-Unix-Timestamp-To-MySQL-Timestamp.html
* https://www.virtualmin.com/node/21032
* https://github.com/felixge/node-mysql/issues/591
* http://stackoverflow.com/questions/22152992/cannot-connect-to-mysql-db-with-nodejs-openshift-app
* http://stackoverflow.com/questions/13437066/need-assistance-with-accessing-mysql-database-using-node-js
* http://stackoverflow.com/questions/23432602/access-denied-while-connecting-to-mysql-through-node-js
* http://www.webhostingtalk.com/showthread.php?t=1446004
* http://ubuntuforums.org/showthread.php?t=1988939
* http://dev.mysql.com/doc/refman/5.5/en/adding-users.html
* http://dev.mysql.com/doc/refman/5.7/en/option-files.html
* http://serverfault.com/questions/483964/mysql-root-problems-access-denied-for-root-user?rq=1
* http://stackoverflow.com/questions/6861355/mysqldump-launched-by-cron-and-password-security/6861458#6861458
* https://bbs.archlinux.org/viewtopic.php?id=182470
* http://serverfault.com/questions/671759/cronjob-executing-mysql-and-get-root-access-denied
* http://stackoverflow.com/questions/3032054/how-to-remove-mysql-root-password
* http://stackoverflow.com/questions/11155489/mysql-the-right-syntax-to-use-near-at-line-1-error
* http://stackoverflow.com/questions/14069256/mysql-timestamp-comparison
* http://stackoverflow.com/questions/11448068/mysql-error-code-1175-during-update-in-mysql-workbench
* http://stackoverflow.com/questions/11912221/mysql-select-by-newest-timestamp
* http://stackoverflow.com/questions/26154243/catch-handle-mysql-duplicate-entry-error-with-nodejs-passportjs-express-con
* http://stackoverflow.com/questions/29051490/not-able-to-store-more-than-20-tweets-to-database-using-twitter-4j-and-java
* http://www.w3schools.com/php/php_mysql_update.asp
* http://ubuntuforums.org/showthread.php?t=1433024

Markdown:
* https://blog.ghost.org/markdown/

Logo images:
* https://c1.staticflickr.com/8/7277/7651088336_1c8cf7d664.jpg
* http://innovationtoday.net/wp-content/uploads/2016/01/Inno-Today-IBM-Logo-300x300.jpg
* https://upload.wikimedia.org/wikipedia/commons/thumb/2/21/Pacific_Gas_and_Electric_Company_(logo).svg/2000px-Pacific_Gas_and_Electric_Company_(logo).svg.png
* http://logok.org/wp-content/uploads/2015/02/Marriott-logo.png
* http://www.bp.com/content/dam/bp/images/general/graphics/1-1/bp-logo.jpg
* https://upload.wikimedia.org/wikipedia/commons/thumb/4/44/Microsoft_logo.svg/2000px-Microsoft_logo.svg.png
* http://vignette4.wikia.nocookie.net/logopedia/images/5/59/Coca-Cola_logo_2007.jpg/revision/latest?cb=20150801090518
* http://www.underconsideration.com/brandnew/archives/att_2016_globe_detail.png
* http://alliancehh.org/wp-content/uploads/2014/01/l92820-wells-fargo-logo-88711.png
* http://logonoid.com/images/unitedhealth-logo.png