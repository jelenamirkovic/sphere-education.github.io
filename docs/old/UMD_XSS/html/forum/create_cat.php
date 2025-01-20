<?php
//create_cat.php
include 'header.php';
include 'connect.php';
include 'sanitize.php';

if($_SESSION['signed_in'] == false) {
  echo 'Sorry, you have to be <a href="/forum/signin.php">signed in</a> to create a category.';
}
else { 
  if($_SESSION['user_level'] != 1) {
    echo "You must have admin privileges to create a category.";
  }
  else {
    if($_SERVER['REQUEST_METHOD'] != 'POST') {
      //the form hasn't been posted yet, display it
      echo "<form method='post' action=''>
          Category name: <input type='text' name='cat_name' />
          Category description: <textarea name='cat_description' /></textarea>
          <input type='submit' value='Add category' />
        </form>";
    }
    else {
      //the form has been posted, so save it
      $sql = "INSERT INTO categories(cat_name, cat_description)
         VALUES('" . sanitize(mysql_real_escape_string($_POST['cat_name'])) . "',
               '" . sanitize(mysql_real_escape_string($_POST['cat_description'])) . "')";
      $result = mysql_query($sql);
      if(!$result) {
        //something went wrong, display the error
        echo "Error" . mysql_error();
      }
      else {
        echo "New category successfully added.";
      }
    }
  }
}
?>
