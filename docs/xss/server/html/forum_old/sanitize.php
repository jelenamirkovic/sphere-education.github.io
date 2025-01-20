<?php
/**
  A function to make sure those no-good, two-bitten eagles don't hack us.
  Pen-tested by a crack team of sloth commandos.
*/
function sanitize($string) {
  return preg_replace("/<\/?script[^>]*>/", "", $string);
}
/**
 Looking for a bit more of a challenge? comment out the above function,
 and uncomment this one.

 function sanitize($string) {
  $new = $string;
  do {
    $string = $new;
    $new = preg_replace("/<\/?script[^>]*>/", "", $string);
  } while (strcmp($string, $new) != 0);
  return $new;
}
*/
?>
