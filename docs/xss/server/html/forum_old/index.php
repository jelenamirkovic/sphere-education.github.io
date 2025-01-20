<?php
//create_cat.php
include 'connect.php';
include 'header.php';
 
$sql = "SELECT
            cat_id,
            cat_name,
            cat_description
        FROM
            categories";
 
$result = mysql_query($sql);
 
if(!$result)
{
    echo "The categories could not be displayed, please try again later.";
}
else
{
    if(mysql_num_rows($result) == 0)
    {
        echo "No categories defined yet.";
    }
    else
    {
        //prepare the table
        echo "<table border='1'>
              <tr>
                <th>Category</th>
                <th>Last topic</th>
              </tr>"; 

        while($row = mysql_fetch_assoc($result))
        {
	    $sql = "SELECT * FROM topics WHERE topic_cat = " . $row["cat_id"] . " ORDER BY topic_date DESC";
	    $topics_result = mysql_query($sql);
	    $topics = mysql_fetch_assoc($topics_result);
            echo "<tr>";
                echo "<td class='leftpart'>";
                    echo "<h3><a href='category.php?id=" . $row["cat_id"] . "'>" . $row["cat_name"] . "</a></h3>" . $row["cat_description"];
                echo "</td>";
                echo "<td class='rightpart'>";
		if (!$topics) {
                    echo "No topics yet";
		}
		else {
		    echo "<a href='topic.php?id=" . $topics["topic_id"] . "'>" . $topics["topic_subject"] . "</a> posted on " . $topics["topic_date"];
		}
		
                echo "</td>";
            echo "</tr>";
        }
    }
}
 
include 'footer.php';
?>
