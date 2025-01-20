var URL = 'http://10.0.1.1';
var webpage = require('webpage');
var system = require('system');
var args = system.args;
var page = webpage.create();
var postBody = 'user_name=root&user_pass=root';
page.onResourceError = function(resourceError) {
    page.reason = resourceError.errorString;
    page.reason_url = resourceError.url;
};
page.open(URL + '/signin.php', 'POST', postBody, function(status) { if ( status !== 'success' ) {
            console.log(
                "Error opening url \"" + page.reason_url
                + "\": " + page.reason
            );
        } else {
            console.log( "Successful page open!" );
        }
});
setTimeout( function() {
page.open(URL + '/topic.php?id=' + args[1], function(status) { if ( status !== 'success' ) {
            console.log(
                "Error opening url \"" + page.reason_url
                + "\": " + page.reason
            );
            phantom.exit( 1 );
        } else {
            console.log( "Successful page open! url: " + page.url);
            // console.log(page.content);
            phantom.exit( 0 );
        }
}) }, 5000);