app.controller('logoutController', function($scope, $cookies) {
	$scope.logout = function() {

        var response = $http.post("/RoSA/logout");
        response.then(
            function (response) {
                $cookies.remove('sessionId');
                $scope.context.loggedIn = false;
                alert("Wylogowano!");
            },
            function (response) {
                alert("Błąd w czasie wylogowania!");
            });
	}
});