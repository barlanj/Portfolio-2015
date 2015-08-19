<?php
    
    if (session_status() === PHP_SESSION_NONE){session_start();}
    include_once('db_connect.php');

    $loginAuthorised = isset($_SESSION['auth']) ? ($_SESSION['auth'] == 1) : false;

    if(!$loginAuthorised)
        header("Location: index.php?status=logout");


    $username = isset($_SESSION['username']) ? $_SESSION['username'] : null;
    $fullname = isset($_POST['fullname']) ? $_POST['fullname'] : null;
    $userid = null;

    //find the user in the database table
    $user_query = "SELECT ID FROM final_userLogin WHERE username = '$username'";
    $result = $mysqli->query($user_query);
    if($result->num_rows > 0) {
        $row = $result->fetch_array(MYSQL_ASSOC);
        $userid = $row['ID'];
    } else {
        $userid = null;
    }
    $result->close();

    //create new card data using ID
    $query =  "INSERT INTO final_userCards (userID, fullname) VALUES ('$userid', '$fullname')";
    $result = $mysqli->query($query);
    
?>