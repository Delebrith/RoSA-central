app.controller("appController", function($scope, $http, $cookies) {
	$scope.context = {
			user: undefined,
			loggedIn: false
	}


	$scope.isLogged = function () {
		return $scope.context.loggedIn;
		// return $cookies.get("session_id") != null
	}

});