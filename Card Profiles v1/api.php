<?php

    if (session_status() === PHP_SESSION_NONE){session_start();}
    $loginAuthorised = isset($_SESSION['auth']) ? ($_SESSION['auth'] == 1) : false;

    if(!$loginAuthorised)
        header("Location: index.php?status=logout"); 

    include_once('db_connect.php');
    $jsondata = array();
    if(isset($_GET['publicID']))
    {
        if($_GET['publicID'] == 'all') //return all public
        {
            $query = "SELECT data_id, imageName, fullname, title, address, email, linkedin, github, back, public FROM final_userCards WHERE public=1";
            $result = $mysqli->query($query);
            while($row = $result->fetch_array(MYSQL_ASSOC)) {
                $jsondata[] = $row;
            }
        }
        else
        {   
            if(is_numeric($_GET['publicID'])) //check if its an id, return specific id data if it's public
            {
                $data_id = $_GET['publicID'];
                $query = "SELECT data_id, imageName, fullname, title, address, email, linkedin, github, back, public ".
                            "FROM final_userCards WHERE public=1 AND data_id='$data_id'";
                $result = $mysqli->query($query);
                while($row = $result->fetch_array(MYSQL_ASSOC)) {
                    $jsondata[] = $row;
                }                  
            }
            
        }
        
    }

    echo json_encode($jsondata)
   
?>