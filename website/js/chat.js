Vue.createApp({
    data () {
        return {
            chatList: [],
            username: '',
            inputText: '',
        }
    },
    computed: {
        chatListSorted () {
            return this.chatList.sort();
        },
        chatSize () {
            return this.chatList.length;
        }
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
                _this.chatSize = _this.chatList.length;
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
                user: _this.username,
                content: _this.inputText,
            };

            fetch('/api/chat', {
                body: JSON.stringify(data),
                method: 'POST',
            })
            .then(resp => {
                if (!resp.ok) {
                    _this.hasError = true;
                    return;
                }

                resp.json()
                .then(data => {
                    // TODO: more efficient way to update
                    _this.updateAllChat();

                    console.log(data);
                    _this.hasError = false;
                    _this.inputText = '';
                });
            });
        }
    },
    created () {
        let _this = this;
        _this.updateAllChat();

        setInterval(_this.updateChatIfServerIsNewer, 1000);
    }
}).mount('#chat-app');
