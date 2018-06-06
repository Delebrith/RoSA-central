app.factory("CORSInterceptor", [
    function()
    {
        return {
            request: function(config)
            {
                config.headers["Access-Control-Allow-Origin"] = "*";
                config.headers["Access-Control-Allow-Methods"] = "GET, POST, DELETE, OPTIONS";
                config.headers["Access-Control-Allow-Headers"] = "Content-Type, access-control-allow-headers, " +
                    "access-control-allow-origin, access-control-allow-methods, Accept";
                config.headers["Access-Control-Request-Headers"] = "X-Requested-With, accept, content-type";
                return config;
            }
        };
    }
]);

app.config(["$httpProvider", function ($httpProvider) {
    // $httpProvider.interceptors.push("CORSInterceptor");
}]);

var sc;
var browsr;
var cookies;
var cstore;

app.controller("appController", function($scope, $http, $cookieStore, $cookies, $browser, $location) {
    browsr = $browser;
    sc = $scope;
    cstore = $cookieStore;
	cookies = $cookies;
    $scope.context = {
			user: undefined
	}

	$scope.isLogged = function () {
		return sessionStorage.getItem("username") != undefined;
	}

	$scope.serverAddress = "http://" + self.location.hostname + ":8080";

});