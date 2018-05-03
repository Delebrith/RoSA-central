app.controller('loginController', function($scope, $http, $cookies, $window) {
	function invalidEmail()
	{
		alert('Niepoprawny adres email!');
	}

	function failedLogin(response)
	{
		alert('Nieudane logowanie!');
	}

	function succesfulLogin(response)
	{
		$cookies.put('session_id', response.headers("session_id"));
		$window.location.reload();
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
		
	    // var response = $http.post("/RoSA/login", userCredentialsDto);
	    // response.then(
	    // 	function(response) {
			// 	succesfulLogin(response);
	    // 	},
	    // 	function(response){
			// 	failedLogin(response);
    	// 	});

		$scope.context.loggedIn = true;
		$scope.context.user = {
            username: $scope.username,
            password: $scope.password
        };
	}
});