package state

import (
	"sync"
)

type SharedState struct {
	WritingData bool
	StateMutex  sync.Mutex
}
