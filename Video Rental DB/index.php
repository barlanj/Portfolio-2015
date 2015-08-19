<?php
include 'data.php';
include 'addVideo.php';
?>

<html>
<head>
	<meta charset="UTF-8">
	<title> cs290 Assignment 4 - Part 2 </title>
	<link href="style.css" rel="stylesheet">
</head>

<body>
	<div id="headerContainer">
		<div id="headerContainer_content">
			<form method="POST" id="userForm" action="index.php">
				<label> Title: <input type="text" name="media_title" maxLength="255"> </label> </br></br>
				<label> Category: <input type="text" name="media_category" maxLength="255"> </label></br></br>
				<label> Length (min): <input type="number" name="media_length" min="1" max="1000"> </label></br></br>
				</br>
				<input type="submit" name="addRecord" value="Add to DB">
				<input type="submit" name="deleteAllRecords" value="Delete All Records">
			</form>
		</div>
	</div>

	<?php 
		include 'showResult.php';
	?>

</body>
</html>