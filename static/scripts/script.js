

const send = document.querySelector("#send");
const message = document.querySelector("#Message");

send.addEventListener('click', async () => {
  let data = message.value
  let response = await fetch("/send", {
    method: 'POST', headers: {
      'Content-type': 'application/json'
    },
    body: JSON.stringify({message: data})
  })
  console.log("log");
  console.log(response);
})

message.addEventListener('keydown', async (e) => {
  if (e.key === 'Enter') {
    let data = message.value
    let response = await fetch("/send", {
      method: 'POST', headers: {
        'Content-type': 'application/json'
      },
      body: {message : data}
    })
    console.log("log");
    console.log(response);
  }
})
