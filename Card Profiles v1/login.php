<?php
    
        include_once('db_connect.php');
        
        $username = isset($_POST['username']) ? $mysqli->real_escape_string($_POST['username']) : null;
        $password = isset($_POST['password']) ? md5($mysqli->real_escape_string($_POST['password'])) : null;
        $username_Retrieved = null;
        $password_Retrieved = null;
        $msg = "";


        if(isset($username) && isset($password)){

            if($username == null || $password == null) {
                $msg .= "Missing data fields.";   
            } else {

                //find the user in the database table
                $user_query = "SELECT username FROM final_userLogin WHERE username = '$username'";

                $result = $mysqli->query($user_query);
                if($result->num_rows > 0) {
                    $row = $result->fetch_array(MYSQL_ASSOC);
                    $username_Retrieved = $row['username'];
                } else {
                    $username_Retrieved = null;
                }
                $result->close();

                //if username found, check password
                if($username_Retrieved != null) {
                    $pass_query = "SELECT password FROM final_userLogin WHERE username = '$username'";

                    $result = $mysqli->query($pass_query);

                    if($result->num_rows > 0) {
                        $row = $result->fetch_array(MYSQL_ASSOC);
                        $password_Retrieved = $row['password'];
                    } 
                    $result->close();
                }

                //check if input matches database
                if($username == $username_Retrieved && $password == $password_Retrieved) {
                    if (session_status() === PHP_SESSION_NONE){session_start();}
                    $_SESSION['auth'] = 1;
                    $_SESSION['username'] = $username;
                    $_SESSION['userIP'] = $_SERVER['REMOTE_ADDR'];
                    $msg .= "success";

                } else {
                    $msg .= "Invalid ID/Pass.";
                }
            }  
            echo $msg;
            return;
        }

?>
