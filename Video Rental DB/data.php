<?php
//error checks
ini_set('display_errors', 'On');
include 'stored-info.php';

//try to connect to database
$mysqli = new mysqli($dbhost, $dbuser, $dbpass, $dbname);
if ($mysqli->connect_errno) {
	echo "Failed to connect to database: (" . $mysqli->connect_errno . ") " . $mysqli->connect_error;
	die("error");
}
//check if table with content exist, if not then create a blank one
$query = "SELECT id FROM movieDB";
$result = $mysqli->query($query);
if(!$result) {
	echo "No db found, creating a new one. </br>";
	$query =  	"CREATE TABLE movieDB (
				id INT NOT NULL AUTO_INCREMENT PRIMARY KEY,
				name VARCHAR(255) NOT NULL UNIQUE,
				category VARCHAR(255),
				length INT,
				rented INT NOT NULL
				)";
	if(!$mysqli->query($query)) {
		echo "Table creation failed: " . $mysqli->errno . " | " . $mysqli->error;
	}
}

?>