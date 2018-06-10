<!--created by p.szwed-->
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
		// $cookies.put('session_id', "some-session-id");
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
		
	    var response = $http({
	    	method: "POST",
	    	url: "/RoSA/login",
			data: userCredentialsDto,
			headers: {"Content-Type" : "application/json"}
		});
	    response.then(
	    	function(response) {
				succesfulLogin(response);
				sessionStorage.setItem("username", userCredentialsDto.username);
                $scope.start();
	    	},
	    	function(response){
				failedLogin(response);
    		});

	}
});