<?php
	$numaccts = 89;
	$maxbal = 14325;
	$minbal = -237; // max neg bal
	$Pnegative = "10"; // prob% acct is neg
	$minssn = 100000000;
	$minyear = 3045;
	$maxyear = 3072;
	$nummonths = 12;
	$maxday = 28;
	
	$names_first = "dist.all.first";
	$names_last = "dist.all.last";
	$passwords = "passwords";


	$username = "root";
	$password = "zubaz99";
	$dbname = "fccu";
	$dbhost = "localhost";
	$dbuser = $dbname;
	$dbpass = $dbname . 'bucks';

	$link_id = mysql_connect($dbhost, $username, $password);
	
	echo "Created database connection.\n";

	if(!mysql_query("CREATE DATABASE $dbname")) die(mysql_error());
	echo "Created database.\n";

	if(!mysql_query("GRANT ALL PRIVILEGES ON $dbname.* TO '$dbuser'@'localhost' IDENTIFIED BY '$dbpass' WITH GRANT OPTION")) die(mysql_error());

	$link_id = mysql_connect($dbhost, $dbuser, $dbpass);
	mysql_select_db($dbname) or die ("Unable to select database.\n");
	$query = "CREATE TABLE accounts (id INT KEY NOT NULL AUTO_INCREMENT, bal FLOAT, first VARCHAR(30) NOT NULL, last VARCHAR(30) NOT NULL, phone VARCHAR(30) NOT NULL, ssn VARCHAR(11) NOT NULL, bday INT NOT NULL, email VARCHAR(30) NOT NULL, password VARCHAR(30) NOT NULL)";
	if (!mysql_query($query)) die(mysql_error());

	// AUTOMATICALLY GENERATE CUSTOMERS
	// shuffle name lists for "random" access later
	$firstnames = file($names_first);
	$lastnames = file($names_last);
	shuffle($firstnames);
	shuffle($lastnames);
	
	$ssn_log[] = "234-00-2967"; // will hold all ssns for uniqueness
	$email_log[] ="";
	for ($i = 0; $i <= $numaccts; $i++) {
		echo "$i : ";
		if ((rand()%100) <= $Pnegative) { // Pnegative = probability
			$this_balance = (rand()%$minbal) * -1; // negative
		} else {
			$this_balance = (rand()%$maxbal); // positive
		}
		echo "$this_balance : ";
		
		$this_lastname = chop($lastnames[$i]); // without newline
		$this_firstname = chop($firstnames[$i]);
		echo "$this_lastname, $this_firstname : ";

		$this_phone = (rand()%2999) + 1000; // from 1000 - 3999
		echo "$this_phone : ";

		// social security numbers with 00 in any section 
		// are invalid; so xxx-00-xxxx is invalid.
		$this_ssn = "234-00-2967"; // already in ssn_log
		while (in_array($this_ssn, $ssn_log)) {
			$this_ssn = strval((rand()%899)+100) . "-00-" . strval((rand()&8999)+1000); 
		}
		$ssn_log[] = $this_ssn;
		echo "$this_ssn : ";
	
		//bday
		$this_year = (rand()%20) + 3245;
		$this_month = (rand()%11) + 1;
		$this_day = (rand()%29) + 1;
		$this_bday = sprintf("%d%02d%02d", $this_year, $this_month, $this_day);

		echo "$this_bday : ";

		$j = 1; // will use for email address similarity
		$this_email = strtolower($this_firstname);
		while (in_array($this_email, $email_log)) {
			$this_email = strtolower($this_firstname) . (string)$j;
		}
		$email_log[] = $this_email;
		echo "$this_email";

		// password selection
		$password_list = file($passwords);
		shuffle($password_list);
		$this_password = chop($password_list[$i]);
		
		echo "$this_password";
		echo "\n";

		$query = "INSERT INTO accounts VALUES('', '$this_balance', '$this_firstname', '$this_lastname', '$this_phone', '$this_ssn', '$this_bday', '$this_email', '$this_password')";
		if (!mysql_query($query)) die(mysql_error());
		
	}
	
	mysql_close();
	
?>
