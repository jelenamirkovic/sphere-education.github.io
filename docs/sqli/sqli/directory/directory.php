<html>
<head>
<title>
FrobozzCo International Company Directory
</title>
</head>
<body>
<h1>FrobozzCo International Company Directory</h1>
<h4>Reach out and touch someone!</h4>
<hr>
<?php

$thispage = "directory.php";
$dbuser = "fccu";
$dbname = $dbuser;
$dbpass = "fccubucks";
$dbhost = "localhost";

$PARAM = array_merge($_GET, $_POST);

if ($PARAM['first']) {
	dirlookup("first", $PARAM['first']);
} elseif ($PARAM['last']) {
	dirlookup("last", $PARAM['last']);
} elseif ($PARAM['phone']) {
	dirlookup("phone", $PARAM['phone']);
} else { // print out form
	lookup_form();
}

function lookup_form() {

	global $thispage;

	echo "<p>Enter ONE of firstname, lastname, OR phone extension and click \"submit.\" Click \"reset\" to clear form.</p>\n";
	echo "<form action='$thispage' method='post'>\n";
	echo "<table>\n";
	echo "<tr><td>First name: </td><td><input name='first' cols='30' /></td></tr>\n";
	echo "<tr><td>Last name: </td><td><input name='last' cols='30' /></td></tr>\n";
	echo "<tr><td>Extension: </td><td><input name='phone' cols='30' /></td></tr>\n";
	echo "<tr><td><button name='submit'>Submit</submit></td><td></td></tr>\n";
	echo "</table>\n";
	echo "</form>\n";
}

function dirlookup($colname, $term) {

	global $dbhost, $dbuser, $dbpass, $dbname;
	
	$link_id = mysql_connect($dbhost, $dbuser, $dbpass) or die(mysql_error());
	mysql_select_db($dbname);
	
	$query = "SELECT * FROM accounts WHERE $colname LIKE '$term'";
	$result = mysql_query($query) or die(mysql_error());

	if ($result) {
		echo "<h2>Result:</h2>\n";
		echo "<table border=1 padding=2>\n";
		echo "<tr><td>First</td><td>Last</td><td>Phone</td><td>Email</td></tr>\n";
		while ($row = mysql_fetch_array($result)) {
			echo "<tr><td>$row[2]</td><td>$row[3]</td><td>$row[4]</td><td>$row[7]@frobozzco.com</td></tr>\n";
		}
		echo "</table>\n";
		echo "<p><b>End of result.</b></p>\n";
		lookup_form();
		mysql_free_result($result);
	} else {
		echo "<h2>No match.</h2>\n";
		lookup_form();
	}
	mysql_close();
}

?>
</body>
</html>
