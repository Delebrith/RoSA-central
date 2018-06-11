<!--created by p.szwed-->
app.controller("appController", function($scope, $http, $cookieStore, $cookies, $browser, $location) {
    sc = $scope;
    $scope.context = {
			user: undefined
	}

	$scope.isLogged = function () {
		return sessionStorage.getItem("username") != undefined;
	}

	$scope.serverAddress = "http://" + self.location.hostname + ":8080";

});