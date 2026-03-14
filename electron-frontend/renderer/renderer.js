import { login, signup } from "../api.js";

// Elements
const startup = document.getElementById("startup");
const loginScreen = document.getElementById("loginScreen");
const signupScreen = document.getElementById("signupScreen");
const customerDash = document.getElementById("customerDash");
const adminDash = document.getElementById("adminDash");

let loginMode = "customer";

// --- Screen Navigation ---
function showScreen(target) {
    const screens = [startup, loginScreen, signupScreen, customerDash, adminDash];

    screens.forEach(s => s.classList.add("hidden"));
    target.classList.remove("hidden");

    // GSAP fade-in animation
    gsap.fromTo(target, { opacity: 0 }, { opacity: 1, duration: 0.8 });
}

// Startup buttons
document.getElementById("customerBtn").onclick = () => {
    loginMode = "customer";
    document.getElementById("goSignup").classList.remove("hidden");
    showScreen(loginScreen);
};

document.getElementById("adminBtn").onclick = () => {
    loginMode = "admin";
    document.getElementById("goSignup").classList.add("hidden");
    showScreen(loginScreen);
};

// Signup button from login
document.getElementById("goSignup").onclick = () => {
    showScreen(signupScreen);
};

// --- LOGIN ---
document.getElementById("loginSubmit").onclick = async () => {
    const u = document.getElementById("loginUser").value;
    const p = document.getElementById("loginPass").value;
    const err = document.getElementById("loginError");

    err.innerText = ""; // clear old errors

    const res = await login(u, p);

    if (!res.success) {
        err.innerText = res.error;

        gsap.fromTo("#loginScreen",
            { x: -10 },
            { x: 10, duration: 0.1, yoyo: true, repeat: 5 }
        );

        return;
    }

    err.style.color = "green";
    err.innerText = `Login successful (${res.role})`;

    gsap.to("#loginScreen", { backgroundColor: "#e1ffd4", duration: 0.5 });

    setTimeout(() => {
        if (res.role === "customer") {
            showScreen(customerDash);
        } else {
            showScreen(adminDash);
        }
    }, 600);
};

// --- SIGNUP ---
document.getElementById("signupSubmit").onclick = async () => {
    const data = {
        name: document.getElementById("sName").value,
        email: document.getElementById("sEmail").value,
        username: document.getElementById("sUser").value,
        password: document.getElementById("sPass").value,
        creditCardNumber: document.getElementById("sCard").value
    };

    const err = document.getElementById("signupError");
    err.innerText = "";

    const res = await signup(data);

    if (!res.success) {
        err.innerText = res.error;

        gsap.fromTo("#signupScreen",
            { x: -10 },
            { x: 10, repeat: 4, duration: 0.1, yoyo: true }
        );

        return;
    }

    err.style.color = "green";
    err.innerText = `Account created! Your ID: ${res.customerID}`;

    gsap.to("#signupScreen", { backgroundColor: "#d4ffe7", duration: 0.5 });

    setTimeout(() => {
        showScreen(customerDash);
    }, 800);
};

// clear errors when typing
document.getElementById("loginUser").oninput = () => loginError.innerText = "";
document.getElementById("loginPass").oninput = () => loginError.innerText = "";

document.querySelectorAll("#signupScreen input")
.forEach(i => i.oninput = () => signupError.innerText = "");