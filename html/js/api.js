
var api = {};


api.comment_add = function(request_data) {
    return $.ajax({ url: "./api/comment-add/", type: 'post', cache: false, dataType: 'json',
        data: JSON.stringify( request_data ),
        async:true,
    })
}

api.versions_all = function(request_data) {
    request_data = request_data || {}
    return $.ajax({ url: "./api/versions-all/", type: 'post', cache: false, dataType: 'json',
        data: JSON.stringify( request_data ),
        async:true,
    })
}
