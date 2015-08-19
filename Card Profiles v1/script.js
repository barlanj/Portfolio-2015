function createDiv(className, textContent)  {
    var div = document.createElement('div');
    div.setAttribute('class', className);
    
    if(textContent != "" || textContent.length > 0) {
        var txt = document.createTextNode(textContent);
        div.appendChild(txt);
    }

    return div;
}

function createDivElements(objArr, divParent) { //accepts [obj, obj, ...] & the parent node to attach to
    
    //find the parent
    var parentContainer = document.getElementById(divParent);
    
    //fields
    var domain = "http://web.engr.oregonstate.edu/~barlanj/final";
    var fullname = "";
    var title = "";
    var address = "";
    var email = "";
    var linkedin = "";
    var githhub = "";
    var back = "";
    var image = ""
    
    var frag = document.createDocumentFragment();
    
    for(var item in objArr) {
        fullname = objArr[item].fullname;
        title = objArr[item].title;
        address = objArr[item].address;
        email = objArr[item].email;
        linkedin = objArr[item].linkedin;
        github = objArr[item].github;
        back = objArr[item].back;
        image = domain + objArr[item].imageName;
        
        
        //left side of card
        var userCard_photo = createDiv('userCard_photo', "");
        userCard_photo.style.backgroundImage = "url('" + image +"')";
        userCard_photo.style.backgroundRepeat = "no-repeat";
        userCard_photo.style.backgroundSize = "cover";
        
        var userCard_fullname = createDiv('p_userCard_fullname', fullname);
        var userCard_title = createDiv('p_userCard_title', title);

        var card_left = createDiv('card_left', "");
        card_left.appendChild(userCard_photo);
        card_left.appendChild(userCard_fullname);
        card_left.appendChild(userCard_title);

        ///////////////////// CARD_RIGHT
        var userCard_address = createDiv('p_userCard_address', address);
        var userCard_email = createDiv('p_userCard_email', email);

        var userCard_link_linkedin = createDiv('p_userCard_link_linkedin', "");
        
        var linkedin_icon = document.createElement('img');
        linkedin_icon.setAttribute('src', 'site_images/icon_linkedin.png');
        linkedin_icon.setAttribute('class', 'icon_linkedin');
        
        var linkedin_icon_container = createDiv('p_icon_container', "");
        linkedin_icon_container.appendChild(linkedin_icon);
        
        var linkedIn_Content = createDiv('p_linkedinContent', linkedin);
        userCard_link_linkedin.appendChild(linkedin_icon_container);
        userCard_link_linkedin.appendChild(linkedIn_Content);

        var userCard_link_github = createDiv('p_userCard_link_github', "");
        
        var github_icon = document.createElement('img');
        github_icon.setAttribute('src', 'site_images/icon_github.png');
        github_icon.setAttribute('class', 'icon_github');
        
        var github_icon_container = createDiv('p_icon_container', "");
        github_icon_container.appendChild(github_icon);
        
        var github_Content = createDiv('p_githubContent', github);
        userCard_link_github.appendChild(github_icon_container);
        userCard_link_github.appendChild(github_Content);

        var userCard_links = createDiv('userCard_links', "");
        userCard_links.appendChild(userCard_link_linkedin);
        userCard_links.appendChild(userCard_link_github);

        var card_right = createDiv('card_right', "");
        card_right.appendChild(userCard_address);
        card_right.appendChild(userCard_email);
        card_right.appendChild(userCard_links);
        ///////////////////////

        var card_frontFace = createDiv('frontFace', "");
        card_frontFace.appendChild(card_left);
        card_frontFace.appendChild(card_right);

        var userCard_back = createDiv('p_userCard_back', back);

        var card_backFace = createDiv('backFace', "");
        card_backFace.appendChild(userCard_back);

        var userCard = createDiv('userCard', "");
        userCard.appendChild(card_frontFace);
        userCard.appendChild(card_backFace);

        frag.appendChild(userCard);  
        
    }
    
    parentContainer.appendChild(frag);
}

$(document).ready( function() {

    $("input#fileUploadButton").click( function() {
       
        $.post( $("#uploadForm").attr("action"),
                $("#uploadForm :input").serializeArray(),
                function(data) {
                    var result = jQuery.parseJSON(data);
                    console.log(result);
            });

        $("#uploadForm").submit( function() {
            return false;	
        });
    });
    
    
    $("button#loginButton").click( function() {
        $("div#loginErrorDiv").empty();
      if( $("#username").val() == "" || $("#password").val() == "" )
        $("div#loginErrorDiv").html("Please enter both username and password");
      else
        $.post( $("#loginForm").attr("action"),
                $("#loginForm :input").serializeArray(),
                function(data) {
                  if(data == 'success') {
                    window.location.replace('index.php?status=loggedin');
                    return true;
                  }
                  $("div#loginErrorDiv").html(data);
            });

        $("#loginForm").submit( function() {
            return false;	
        });
    }); 

    
    $("button#registerButton").click( function() {
        $("div#loginErrorDiv").empty();
        if( $("#username").val() == "" || $("#password").val() == "" || $("#email").val() == "")
            $("div#loginErrorDiv").html("Please enter both username and password");
        else {

            $.post($("#registerForm").attr("action"),
                   $("#registerForm :input").serializeArray(),
                   function(data) {
                        var result = jQuery.parseJSON(data);
                        if(result[0] == 'successful registration') {
                            window.location.replace('index.php?status=loggedin');
                        }
                        else {
                            $.each(result, function (i, val) {
                                $("div#loginErrorDiv").append(""+val+"</br>");
                            });
                        }
                    });
        }
        
        $("#registerForm").submit( function() {
        return false;	
            
        });

    }); 
    
    $(".cardInput").keyup( function() {
        var postData = {};
        postData[$(this).attr("name")] = $(this).val();
        $.post("updateCard.php", postData );
        //alert("changed: " + name);
    });
    
    
    $(".onoffswitch-checkbox").change( function() {
        var public = 0;
        if($(this).is(':checked') == true)
            public = 1;
            
        $.post( "updateCard.php", {share: public} );
        
    });
    
    if($("#public_cards_container").html()) {
        $.ajax({
            url: 'api.php',
            type: 'GET',
            data: 'publicID=all',
            success: function(data) {
                var result = jQuery.parseJSON(data);
                createDivElements(result, "public_cards_container" );
            },
            error: function(e) {
                console.log(e.message);
            }

        });
    }
    
    $(".card").click( function () {
        $(this).toggleClass('flipped');
    });

});