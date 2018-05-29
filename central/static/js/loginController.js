app.controller('loginController', function($scope, $http, $cookies, $window) {
	function invalidUsername()
	{
		alert('Niepoprawna nazwa!');
	}

	function failedLogin(response)
	{
		alert('Nieudane logowanie!');
	}

	function succesfulLogin(response)
	{
		$cookies.put('session_id', "some-session-id");
        $scope.context.user = {
            username: $scope.username,
            password: $scope.password
        };
        $scope.checkSensors;
	}

	$scope.submit = function()	{
		var userCredentialsDto = {
			username: $scope.username,
			password: $scope.password
		};
		
		if ($scope.username == undefined) {
			invalidUsername();
			return;
		}
		
	    var response = $http.post($scope.serverAddress + "/RoSA/login", userCredentialsDto);
	    response.then(
	    	function(response) {
				succesfulLogin(response);
	    	},
	    	function(response){
				failedLogin(response);
    		});

	}
});