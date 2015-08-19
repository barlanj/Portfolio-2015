<?php

/************************ ITEM FUNCTIONS **********************************************/
function addItemToTable ($mysqli, $dbname, $title, $category, $length) {
	$queryInsert =  "INSERT INTO  `$dbname`.`movieDB` (`id` ,`name` ,`category` ,`length` ,`rented`)".
					"VALUES (NULL ,  '$title',  '$category',  '$length',  '')";

	if( !$mysqli->query($queryInsert) ) {
		if ($mysqli->errno == 1062) {
			showError( "Duplicate item detected. An Item with the same name exist. Err_No:" . $mysqli->errno );
		} else {
			showError( "Unable to add Item. ErrNo:". $mysqli->errno) ;
		}
    }
}

function clearTable ($mysqli) {
	if(!$mysqli->query("TRUNCATE TABLE movieDB")) {
		showError("Unable to clear records.");
	}
}

function removeItemFromTable ($mysqli, $mediaID) {
	if (! $mysqli->query( "DELETE FROM movieDB WHERE id=$mediaID" )) {
		showError("Deletion failed.");
	}
}

function checkInOrOut($mysqli, $mediaID) {
	if (! $mysqli->query("UPDATE movieDB SET rented = !rented WHERE id=$mediaID")) {
		showError("Could not update location of media.");
	}
}

function showError($message) {
	echo "<div class='errorMsg'>";
	echo "<p class='errorMsg' onclick='this.parentNode.parentNode.removeChild(this.parentNode); return false;'>$message</p>";
	echo "</div>";
}


function showResult($mysqli, $filter) {

	echo "<div id='filterContainer'> <div id='filterContainer_content'>";
		/************************ CREATE DROP DOWN MENU ****************************************/
		//query the database
		$catQuery = $mysqli->prepare("SELECT DISTINCT category FROM movieDB WHERE category IS NOT NULL AND TRIM(category) <> '' ORDER BY category");
		$catElements = NULL;
		$catQuery->bind_result($catElements);
		$catQuery->execute();
		$catQuery->store_result();
		// create the menu
		echo "<form method='POST' id='filterForm' action=''>";
		echo "<select name='filterSelectOption'> 
					<option value='ShowAll'> Show All </option>";
		while($catQuery->fetch()) {
			echo 	"<option value=$catElements> $catElements </option>";
		}
		echo "</select>";
		echo "<button type='submit' value='filterResult' name='filterResult'> Filter </button>";
		echo "</form>";
	echo "</div></div>";



		if($filter === "ShowAll") {
			$filterQuery = "SELECT id, name, category, length, rented 
							FROM movieDB
							ORDER BY id DESC";
		} else {
			$filterQuery = "SELECT id, name, category, length, rented 
							FROM movieDB 
							WHERE category='$filter'
							ORDER BY id DESC";
		}
		$result = $mysqli->prepare($filterQuery);
		$result->execute();

		$res_id = NULL;
		$res_name = NULL;
		$res_category = NULL;
		$res_length = NULL;
		$res_rented = NULL;
		$result->bind_result($res_id, $res_name, $res_category, $res_length, $res_rented);

		echo "<div id='resultContainer'> <div id='resultContainer_content'>";

		$option = isset($_POST['filterSelectOption']) ? $_POST['filterSelectOption'] : false;
		if($option) {
			$text = ($_POST['filterSelectOption'] === "ShowAll" ? 'All' : $_POST['filterSelectOption']);
			echo "Showing: ". $text . "</br>";
		} else {
			echo "Showing: All";
		}

		
		echo "<table>";
		echo "<tr> <th> ID <th> Title <th> Category <th> Length <th> Status <th>  <th>   </tr>";
		while($result->fetch()) {
			$rentedStatus = ($res_rented === 0 ? 'available' : 'checked out');
			$buttonStatus = ($rentedStatus === 'available' ? 'check out' : 'check in');
			echo "<tr>";
				echo "<td>$res_id</td>";
				echo "<td>$res_name</td>";
				echo "<td>$res_category</td>";
				echo "<td>$res_length min</td>";
				echo "<td>$rentedStatus</td>";
				echo "<td><form method='POST' action=''><button type='submit' value=$res_id name='changeStatusRecord' class='button_checkOut'>$buttonStatus</button></form></td>";
				echo "<td><form method='POST' action=''><button type='submit' value=$res_id name='deleteRecord' class='button_delete'>delete</button></form></td>";
			echo "</tr>";
		}
		echo "</table>";
		echo '</div></div>';
}
?>


