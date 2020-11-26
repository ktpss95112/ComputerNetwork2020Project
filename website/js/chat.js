Vue.createApp({
    data () {
        return {
            chatList: [],
            username: '',
            inputText: '',
            hasLoggedIn: false,
            statusMsg: '',
            loginAreaUsername: '',
            loginAreaPassword: '',
        }
    },
    computed: {
        chatListSorted () {
            return this.chatList.sort();
        },
        chatSize () {
            return this.chatList.length;
        },
    },
    methods: {
        updateChatIfServerIsNewer () {
            let _this = this;

            fetch('/api/chat/size')
            .then(resp => resp.json())
            .then(data => {
                if (data.size > _this.chatSize) {
                    this.updateAllChat();
                }
            });
        },
        updateAllChat () {
            let _this = this;

            fetch('/api/chat')
            .then(resp => resp.json())
            .then(data => {
                _this.chatList = data;
            });
        },
        fetchOneChatById (id, callback) {
            fetch(`/api/chat/${id}`)
            .then(resp => resp.json())
            .then(callback);
        },
        send () {
            let _this = this;
            let data = {
                content: _this.inputText,
            };

            fetch('/api/chat', {
                body: JSON.stringify(data),
                method: 'POST',
            })
            .then(resp => {
                if (!resp.ok) {
                    return;
                }

                resp.json()
                .then(data => {
                    // TODO: more efficient way to update
                    _this.updateAllChat();

                    _this.inputText = '';
                });
            });
        },
        register () {
            let _this = this;
            let data = {
                username: _this.loginAreaUsername,
                password: _this.loginAreaPassword,
            };

            _this.statusMsg = 'Processing ... ';

            fetch('/api/register', {
                body: JSON.stringify(data),
                method: 'POST',
            })
            .then(resp => {
                if (!resp.ok) {
                    _this.statusMsg = 'Register failed!';
                }
                else {
                    _this.statusMsg = 'Register Success!';
                }
                _this.loginAreaUsername = '';
                _this.loginAreaPassword = '';
            });
        },
        login () {
            let _this = this;
            let data = {
                username: _this.loginAreaUsername,
                password: _this.loginAreaPassword,
            };

            _this.statusMsg = 'Processing ... ';

            fetch('/api/login', {
                body: JSON.stringify(data),
                method: 'POST',
            })
            .then(resp => {
                if (!resp.ok) {
                    _this.statusMsg = 'Login failed!';
                    return;
                }

                resp.json()
                .then(data => {
                    // set cookie
                    document.cookie = `session_id=${data.session_id}`;
                    _this.hasLoggedIn = true;
                    _this.username = _this.loginAreaUsername;
                    _this.statusMsg = '';
                })
            });
        },
        logout () {
            let _this = this;

            _this.hasLoggedIn = false;
            _this.statusMsg = '';
            _this.loginAreaUsername = '';
            _this.loginAreaPassword = '';

            document.cookie = "session_id=; expires=Thu, 01 Jan 1970 00:00:00 GMT";
        },
    },
    created () {
        let _this = this;
        _this.updateAllChat();

        // update chat every 1 second
        setInterval(_this.updateChatIfServerIsNewer, 1000);

        // check login state
        if (document.cookie.split('; ').some((item) => item.trim().startsWith('session_id'))) {
            // session_id exists
            _this.hasLoggedIn = true;
            let session_id = document.cookie.split('; ').find(row => row.startsWith('session_id')).split('=')[1];

            // update username
            fetch(`/api/username/${session_id}`)
            .then(resp => resp.json())
            .then(data => {
                _this.username = data.username;
            });
        }
    }
}).mount('#chat-app');
