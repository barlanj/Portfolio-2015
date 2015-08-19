<?php

if($_POST) {

	$changeStatusRecord = isset($_POST['changeStatusRecord']) ? $_POST['changeStatusRecord'] : false;
	if($changeStatusRecord) {
		checkInOrOut($mysqli, $_POST['changeStatusRecord']);
		showResult($mysqli, "ShowAll");
	}

	$deleteAllRecords = isset($_POST['deleteAllRecords']) ? $_POST['deleteAllRecords'] : false;
	if ($deleteAllRecords) {
		clearTable($mysqli);
		showResult($mysqli, "ShowAll");
	}

	$filterResult = isset($_POST['filterResult']) ? $_POST['filterResult'] : false;
	if ($filterResult) {
		showResult($mysqli, $_POST['filterSelectOption']);
	}

	$addRecord = isset($_POST['addRecord']) ? $_POST['addRecord'] : false;
	if ($addRecord) {
		$missing = "";
		if(empty($_POST["media_title"]) || !isset($_POST["media_title"]) ){
			$missing = $missing . " [title] ";
		}
		/*if(empty($_POST["media_category"]) || !isset($_POST["media_category"]) ){
			$missing = $missing . " [category] ";
		}*/
		if(empty($_POST["media_length"]) || !isset($_POST["media_length"])){
			$missing = $missing . " [length] ";
		}
		if(!is_numeric($_POST["media_length"]) || $_POST["media_length"] < 1){
			$missing = $missing . " [length must be a positive number]";
		}

		if($missing == ""){
			$title = htmlspecialchars($_POST["media_title"]);
			$category = htmlspecialchars($_POST["media_category"]);
			$length = htmlspecialchars($_POST["media_length"]);

			addItemToTable($mysqli, $dbname, $title, $category, $length);
			showResult($mysqli, "ShowAll");
		} else {
			showError("Missing information: $missing ");
			showResult($mysqli, "ShowAll");
		}
	}

	$deleteRecord = isset($_POST['deleteRecord']) ? $_POST['deleteRecord'] : false;
	if ($deleteRecord) {
		removeItemFromTable ($mysqli, $_POST['deleteRecord']);
		showResult($mysqli, "ShowAll");
	}

} else {
	showResult($mysqli, "ShowAll");
}

$mysqli->close();
?>

