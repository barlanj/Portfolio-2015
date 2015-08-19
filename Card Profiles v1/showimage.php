<?php

    if (session_status() === PHP_SESSION_NONE){session_start();}
    $loginAuthorised = isset($_SESSION['auth']) ? ($_SESSION['auth'] == 1) : false;

    if(!$loginAuthorised)
        header("Location: index.php?status=logout");
    

    if(isset($_GET['imageID']))
    {
        include_once('db_connect.php');
        $imageId = mysql_real_escape_string($_GET['imageID']);
        /*
        $username = isset($_SESSION['username']) ? $_SESSION['username'] : null;
        $userid = null;

        $user_query = "SELECT ID FROM final_userLogin WHERE username = '$username'";
        $result = $mysqli->query($user_query);
        if($result->num_rows > 0) {
            $row = $result->fetch_array(MYSQL_ASSOC);
            $userid = $row['ID'];
        } else {
            $userid = null;
        }
        */
        $query = "SELECT * FROM final_userCards WHERE data_id='$imageId'";
        $result = $mysqli->query($query);
        $row = $result->fetch_array(MYSQL_ASSOC);
        $imageData = $row['profilePhoto'];
        header("content-type: image/jpg");
        
        echo $imageData;
    }
?>