/*jslint plusplus: true, sloppy: true, vars: true, white: true*/
var parsedSearchResult = []; //original search result - unfiltered
var filteredSearchResult = []; // the search result filtered
var favoriteSaves = null; // the localStorage items after parsing

/******************  THE RESULT **************************************************/
function createEntry(gist) {
	//get the file count and the file language types
	var fileLang = [];
	var fileCount = 0;
    
	var i;
	for (i in gist.files) {
		if(!gist.files[i].language) {
            if(fileLang.indexOf('Unknown') === -1) {
                fileLang.push('Unknown');
            }
		}
		else {
            if(fileLang.indexOf(gist.files[i].language) === -1) {
                fileLang.push(gist.files[i].language);
            }
            
		++fileCount;
	   }  
    }
	
    var description = gist.description;
    if (description === "" || description === undefined || description === null) {
        description = "No Description Provided";
    }
    
	//create an object
	var res = {
		'id': gist.id,
		'link': gist.html_url,
		'description': description,
		'lang': fileLang,
		'fileCount': fileCount
	};

	//return the object
	return res;
}


function parseResult(gistArray) {
	var i;
	for(i = 0; i < gistArray.length; ++i) {
		parsedSearchResult.push(createEntry(gistArray[i]));
	}
}

function parseLanguage(entry) {
    var lang;
    var langString = "";
    for(lang in entry) {
        langString += entry[lang] + ', ';
    }
    langString = langString.slice(0, -2);
    
    return langString;   
}


function createDivEntry(entry, location){
    
	//create the elements
	var div = document.createElement('div');
	div.setAttribute('id', entry.id);
	div.setAttribute('class', 'entryContent');

	var files = document.createElement('span');
	files.textContent = entry.fileCount;
	files.setAttribute('class', 'entryFileCount');

	var link = document.createElement('a');
	link.setAttribute('href', entry.link);
	link.textContent = entry.link;
	link.setAttribute('class', 'entryLink');

	var description = document.createElement('p');
	description.textContent = entry.description;
	description.setAttribute('class', 'entryDescription');

	var lang = document.createElement('p');
    if (location === "favoriteContainer") {
        lang.textContent = entry.lang;
    }
    else {
        lang.textContent = parseLanguage(entry.lang);
    }
	
	lang.setAttribute('class', 'entryLanguage');


	var buttonValue = "+";
	var buttonClick = 'saveGist("' + entry.id + '")';
	var buttonClass = 'entrySaveButton';
	if(location === "favoriteContainer") {
		buttonValue = "-";
		buttonClick = 'removeFavoriteEntry("' + entry.id + '")';
		buttonClass = 'entryRemoveButton';
	}
	var saveButton = document.createElement('input');
	saveButton.value = buttonValue;
	saveButton.type = "button";
	saveButton.setAttribute('id', 'entryButton_' + entry.id);
	saveButton.setAttribute('onclick', buttonClick);
	saveButton.setAttribute('class', buttonClass);

    
	div.className = 'gistEntry';
	div.appendChild(saveButton);
	div.appendChild(link);
    div.appendChild(description);
	div.appendChild(lang);
	div.appendChild(files);

	//return the div element
	return div;
}

function showResults(results) {
	var resultBox = document.getElementById("searchResults"); //html container

    //clean up if there's something there from previous search
    var element = resultBox.getElementsByClassName('gistEntry');
    while(element[0]) {
        element[0].parentNode.removeChild(element[0]);
    }
    
	var i, entry;
	for(i = 0; i < results.length; ++i) {
		//create entry element
		entry = createDivEntry(results[i]);
		resultBox.appendChild(entry);
	}
}

function filterResults() {
	var langChecked = [];
	filteredSearchResult = [];

	//get all the languages checked by user and push to langChecked
	var languages = document.getElementsByName("lang");
    var size = languages.length;
    var i;
	for(i = 0; i < size; ++i) {
		if(languages[i].checked) {
			langChecked.push(languages[i].value);
		}
	}

	// see of none checked
	if(langChecked.length < 1) {
		return parsedSearchResult;
	}
    
	// iterate over result array to find matching lang
    var entry, fileType, langC;
    for (entry in parsedSearchResult) {
        //check every files language in the entry
        for (fileType in parsedSearchResult[entry].lang) {
            for (langC in langChecked) {
                if (langChecked[langC] === parsedSearchResult[entry].lang[fileType]) {
                    filteredSearchResult.push(parsedSearchResult[entry]);
                }
            }
        }
    }
    
    return filteredSearchResult;   
}

/******************  FAVORITES **************************************************/
function saveGist(id) {
	var divEntry = document.getElementById(id);
	var parentContainer = document.getElementById("favoriteContainer");
	var firstDivChild = parentContainer.getElementsByTagName('div')[0];


	// put the entry inside the favorite container
	parentContainer.insertBefore(divEntry, firstDivChild);
	var saveButton = document.getElementById('entryButton_' + id);
	saveButton.value = "-";
	saveButton.setAttribute('class', 'entryRemoveButton');
	saveButton.setAttribute('onClick', 'removeFavoriteEntry("' + id + '")');


	var saveEntry = {
		'id': id,
		'link': divEntry.getElementsByTagName('a')[0].getAttribute('href'),
		'description': divEntry.getElementsByClassName('entryDescription')[0].textContent,
		'lang': divEntry.getElementsByClassName('entryLanguage')[0].textContent,
		'fileCount': divEntry.getElementsByClassName('entryFileCount')[0].textContent
	};

	favoriteSaves.gist.unshift(saveEntry);
	localStorage.setItem('userFavoriteGists', JSON.stringify(favoriteSaves));

}

function removeFavoriteEntry(id) {
    
    //return the element to the top of the search listings
    var parent = document.getElementById('searchResults');
    var divEntry = document.getElementById(id);
    var firstDivChild = parent.getElementsByTagName('div')[0];
    parent.insertBefore(divEntry, firstDivChild);

    // change the functionality of the button
	var saveButton = document.getElementById('entryButton_' + id);
	saveButton.value = "+";
	saveButton.setAttribute('class', 'entrySaveButton');
	saveButton.setAttribute('onClick', 'saveGist("' + id + '")');

    
	//find the element index and delete the element from localStorage
    var i;
	for (i in favoriteSaves.gist) {
		if(favoriteSaves.gist[i].id === id) {
			favoriteSaves.gist.splice(i, 1);
			break;
		}
	}
    
    
	//save local storage
	localStorage.setItem('userFavoriteGists', JSON.stringify(favoriteSaves));
}

function showFavorites() {
	var favoriteContainer = document.getElementById('favoriteContainer');
	var firstDivChild = favoriteContainer.getElementsByTagName('div')[0];
	var entryArray = favoriteSaves.gist;

    var gist;
	for(gist in entryArray) {
		var divEntry = createDivEntry(entryArray[gist], "favoriteContainer");

		var idButton = 'entryButton_' + entryArray[gist].id;
		var saveButton = document.getElementById(idButton);
		//saveButton.value = "remove";
		//saveButton.setAttribute('onClick', 'removeFavoriteEntry("' + id + '")');


		favoriteContainer.appendChild(divEntry);
		//favoriteContainer.insertBefore(divEntry, firstDivChild.nextSibling);
	}
}


/*********** START **************************************************************/
function getTheGists() {
	//send the request for gists
	var countValue = document.getElementById("pageCount").value;
    
    //do nothing if not within [1-5]
    if(countValue < 0 || countValue > 5) {
        console.log("attempted to fetch more than 5 pages");
        return;
    }
    
    
	var i, req;
	for(i = 0; i < countValue; ++i) {
		//create a request object and check if successful
		req = new XMLHttpRequest();
		if(!req) {
			throw "Could not create XMLHttpRequest!!";
		}

        req.onreadystatechange = function () {
			if(this.readyState === 4 && this.status === 200) {
				var gists = JSON.parse(this.responseText);
				parseResult(gists);
                var fSearched = filterResults();
				showResults(fSearched);
			}
        };

		var url = 'https://api.github.com/gists?page=' + (i + 1);
		req.open('GET', url);
		req.send();
	}
}

window.onload = function() {
    //set the page number to some default value
    var pageCount = document.getElementById("pageCount");
    pageCount.value = 1;
    
    
	favoriteSaves = localStorage.getItem('userFavoriteGists');
	if(favoriteSaves === null) {
		favoriteSaves = {'gist': []};
		localStorage.setItem('userFavoriteGists', JSON.stringify(favoriteSaves));
	}
	else {
		favoriteSaves = JSON.parse(favoriteSaves);
	}

	showFavorites();
};