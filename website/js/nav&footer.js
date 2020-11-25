const navHTML = `
<nav class="pure-menu pure-menu-horizontal">
    <a href="/" class="pure-menu-heading pure-menu-link">chichichichichi's Website</a>

    <ul class="pure-menu-list">
    <li class="pure-menu-item">
        <a href="/index.html" class="pure-menu-link">Profile</a>
    </li>
    <li class="pure-menu-item">
        <a href="/chat.html" class="pure-menu-link">Chat</a>
    </li>
    <li class="pure-menu-item">
        <a href="/video.html" class="pure-menu-link">Video</a>
    </li>
    </ul>
</nav>
`;


const footerHTML = `
<footer>
    <span>&copy; 2020, Made by Chi-Feng Tsai</span>
    <span><a href="mailto:b07902123@ntu.edu.tw">b07902123@ntu.edu.tw</a></span>
</footer>
`;


const pathname2text = new Map([
    ['/', 'Profile'],
    ['/index.html', 'Profile'],
    ['/chat.html', 'Chat'],
    ['/video.html', 'Video'],
]);


const body = document.querySelector('body');
body.insertAdjacentHTML('afterbegin', navHTML);
body.insertAdjacentHTML('beforeend', footerHTML);


const text = pathname2text.get(window.location.pathname);
const links = document.getElementsByClassName('pure-menu-item');
for (let link of links) {
    if (link.firstElementChild.innerText === text) {
        link.classList.add('pure-menu-selected');
    }
}
