

const send = document.querySelector("#send");
const message = document.querySelector("#Message");



send.addEventListener('click', async () => {
  let data = message.value
  let response = await fetch("/send", {
    method: 'POST', headers: {
      'Content-type': 'text/plain'
    },
    body: data
  })
  console.log("log");
  console.log(response);
})
