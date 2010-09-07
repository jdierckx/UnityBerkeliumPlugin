function callExternalHost(func)
{
	if(typeof(window.externalHost) != 'undefined')
	{
		/*var id = window.idnum++;
		externalHost.postMessage({type:'reply', id: id, ret: ret}, 'about:blank');*/
		window.externalHost.postMessage(func, "about:blank");
	}
}

function callUnityFunction(func, args)
{
	var obj = {func: func, args: args};
	callExternalHost(JSON.stringify(obj));
}

function callUnityFunctionTest()
{
	callUnityFunction('testFunction', {'param1' : 'value1', param2 : 'value2', 'param3' : 123, param4 : 456});
}

window.addEvent('domready', function()
{
	// Input test
	$('inputTest').set('value', "Enter text here");
	$('inputTest').addEvent('click', function() { $('inputTest').select(); });
	$('inputTest').addEvent('change', onInputChanged);
	
	// Button test
	$('buttonTest').addEvent('click', onButtonClicked);

	// Notify the host
	callExternalHost("windowReady");
});

function setInput(value)
{
	$('inputTest').set('value', value);
}
function getInput()
{
	return $('inputTest').get('value');
}

function onInputChanged()
{
	callExternalHost("inputChanged(" + getInput() + ")");
}

function onButtonClicked()
{
	callExternalHost("buttonClicked");
}
